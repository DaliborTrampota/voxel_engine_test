# measure_perf.ps1 — logs CPU (overall + per-core) and GPU utilization to CSV
# Usage: .\measure_perf.ps1 [-IntervalSec 1] [-OutputFile "perf_log.csv"]
param(
    [int]$IntervalSec = 1,
    [string]$OutputFile = "perf_log_$(Get-Date -Format 'yyyyMMdd_HHmmss').csv"
)

$coreCount = (Get-CimInstance Win32_Processor).NumberOfLogicalProcessors
Write-Host "Logging to: $OutputFile"
Write-Host "Sampling every ${IntervalSec}s — press Ctrl+C to stop and see summary."
Write-Host ""

# Build CSV header
$coreHeaders = (0..($coreCount - 1) | ForEach-Object { "Core$_%" }) -join ","
"Timestamp,CPU_Total%,$coreHeaders,GPU_3D%,GPU_Copy%,GPU_Decode%" | Out-File $OutputFile -Encoding utf8

# ── GPU runs in a background runspace so it never blocks the CPU loop ─────────
$gpuState = [hashtable]::Synchronized(@{ gpu3d = 0.0; gpuCopy = 0.0; gpuDec = 0.0 })

$rs = [runspacefactory]::CreateRunspace()
$rs.ApartmentState = 'MTA'
$rs.ThreadOptions  = 'ReuseThread'
$rs.Open()
$rs.SessionStateProxy.SetVariable('gpuState', $gpuState)

$gpuScript = {
    while ($true) {
        $s = (Get-Counter '\GPU Engine(*)\Utilization Percentage' -ErrorAction SilentlyContinue).CounterSamples
        if ($s) {
            $v3d  = ($s | Where-Object { $_.InstanceName -match 'engtype_3d'          } | Measure-Object CookedValue -Sum).Sum
            $vCp  = ($s | Where-Object { $_.InstanceName -match 'engtype_copy'         } | Measure-Object CookedValue -Sum).Sum
            $vDec = ($s | Where-Object { $_.InstanceName -match 'engtype_videodecode'  } | Measure-Object CookedValue -Sum).Sum
            $gpuState.gpu3d   = [math]::Min([math]::Round($v3d,  1), 100)
            $gpuState.gpuCopy = [math]::Min([math]::Round($vCp,  1), 100)
            $gpuState.gpuDec  = [math]::Min([math]::Round($vDec, 1), 100)
        }
    }
}

$ps = [powershell]::Create()
$ps.Runspace = $rs
$null = $ps.AddScript($gpuScript)
$null = $ps.BeginInvoke()   # fire and forget

# ── CPU-only counter paths (fast ~1s) ─────────────────────────────────────────
$cpuPaths = @(
    '\Processor(_Total)\% Processor Time',
    '\Processor(*)\% Processor Time'
)

# Warm up
$null = Get-Counter $cpuPaths -ErrorAction SilentlyContinue
Start-Sleep -Seconds 1

$coreHeader = (0..($coreCount - 1) | ForEach-Object { "C$_".PadLeft(5) }) -join "  "
Write-Host "Timestamp              CPU%  $coreHeader  GPU_3D%"
Write-Host ("-" * (25 + $coreCount * 7 + 10))

$samples    = [System.Collections.Generic.List[double]]::new()
$gpuSamples = [System.Collections.Generic.List[double]]::new()

try {
    while ($true) {
        $ts  = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
        $all = (Get-Counter $cpuPaths -SampleInterval 1 -ErrorAction SilentlyContinue).CounterSamples

        if (-not $all) {
            Write-Host "$ts  [counter read failed, skipping]"
            continue
        }

        $cpuTotal = [math]::Round(
            ($all | Where-Object { $_.Path -match 'Processor\(_Total\)' })[0].CookedValue, 1)

        $cores = $all |
            Where-Object { $_.Path -match 'Processor\(\d' } |
            Sort-Object   { [int]($_.InstanceName) } |
            ForEach-Object { [math]::Round($_.CookedValue, 1) }

        $gpu3d   = $gpuState.gpu3d
        $gpuCopy = $gpuState.gpuCopy
        $gpuDec  = $gpuState.gpuDec

        $samples.Add($cpuTotal)
        $gpuSamples.Add($gpu3d)

        $coreVals = $cores -join ","
        "$ts,$cpuTotal,$coreVals,$gpu3d,$gpuCopy,$gpuDec" | Add-Content $OutputFile -Encoding utf8

        $coreDisp  = ($cores | ForEach-Object { [string]$_ + "%" }) -join "  "
        $cpuDisp   = [string]$cpuTotal + "%"
        $gpuDisp   = [string]$gpu3d    + "%"
        Write-Host "$ts  $($cpuDisp.PadLeft(6))  $coreDisp  $($gpuDisp.PadLeft(7))"

        if ($IntervalSec -gt 1) { Start-Sleep -Seconds ($IntervalSec - 1) }
    }
}
finally {
    $ps.Stop()
    $rs.Close()

    if ($samples.Count -gt 0) {
        $avgCpu = [math]::Round(($samples    | Measure-Object -Average).Average, 1)
        $maxCpu = [math]::Round(($samples    | Measure-Object -Maximum).Maximum, 1)
        $avgGpu = [math]::Round(($gpuSamples | Measure-Object -Average).Average, 1)
        $maxGpu = [math]::Round(($gpuSamples | Measure-Object -Maximum).Maximum, 1)

        Write-Host ""
        Write-Host "=== Summary ($($samples.Count) samples) ==="
        Write-Host "  CPU avg: $avgCpu%   CPU peak: $maxCpu%"
        Write-Host "  GPU(3D) avg: $avgGpu%   GPU(3D) peak: $maxGpu%"
        Write-Host "  Full log: $OutputFile"
    }
}
