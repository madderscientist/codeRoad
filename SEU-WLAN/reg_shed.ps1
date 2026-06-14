# 定义工作目录
$WorkDir = $PSScriptRoot
if (-not $WorkDir) {
    $WorkDir = Get-Location
}

Write-Host "工作目录: $WorkDir" -ForegroundColor Cyan

# 创建批处理文件作为包装器
$BootBat = @"
@echo off
cd /d "$WorkDir"
python "$WorkDir\onboot.py"
"@

$DailyBat = @"
@echo off
cd /d "$WorkDir"
python "$WorkDir\daily.py"
"@

# 保存批处理文件
$BootBatPath = "$WorkDir\run_onboot.bat"
$DailyBatPath = "$WorkDir\run_daily.bat"
$BootBat | Out-File -FilePath $BootBatPath -Encoding ASCII
$DailyBat | Out-File -FilePath $DailyBatPath -Encoding ASCII

# 删除旧任务（如果存在）
schtasks /query /tn "WifiAuth_OnBoot" 2>$null
if ($?) { schtasks /delete /tn "WifiAuth_OnBoot" /f }

schtasks /query /tn "WifiAuth_Daily" 2>$null
if ($?) { schtasks /delete /tn "WifiAuth_Daily" /f }

# 创建开机任务（延迟1分钟）
schtasks /create /tn "WifiAuth_OnBoot" /tr "$BootBatPath" /sc onstart /delay 0001:00 /ru "SYSTEM" /rl highest /f

# 创建凌晨4点任务
schtasks /create /tn "WifiAuth_Daily" /tr "$DailyBatPath" /sc daily /st 04:00 /ru "SYSTEM" /rl highest /f

Write-Host "定时任务创建成功！" -ForegroundColor Green
Write-Host "  - 开机1分钟后: 执行 $BootBatPath" -ForegroundColor Yellow
Write-Host "  - 每天凌晨4点: 执行 $DailyBatPath" -ForegroundColor Yellow