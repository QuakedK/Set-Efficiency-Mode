# Set Efficiency Mode
Set Efficiency Mode is a lightweight C++ console application that lets you enable EcoQos/Efficiency Mode for any application. Users can easily add apps via a simple configuration file and run the with no console!

<img width="1649" height="920" alt="image" src="https://github.com/user-attachments/assets/f4d99932-2b5c-4339-a8f5-5bbe3427f067" />

![GitHub Release Downloads](https://img.shields.io/github/downloads/QuakedK/Set-Efficiency-Mode/total)

# #1 Usage
1. Download and the [Set Efficiency Mode Zip](https://github.com/QuakedK/Set-Efficiency-Mode/releases/download/EfficiencyMode/SetEfficiencyMode.zip).
2. Right-click & extract it.
3. Edit and add the apps of your choosing to the Config.txt.
4. Once down run SetEfficiencyMode.exe as admin!

# #2 Config Example and Format
```
Discord.exe
Spotify.exe
```
# #3 Video Tutorial
https://github.com/user-attachments/assets/afdd52e7-7be8-4be5-ba33-7d972f3a314c

# #4 Command-line arguments
Set Efficiency Mode can be ran with a No Console flag to silently launch itself!
```
:: No Conole Flag
--no-console

:: CMD No Console Launch
"C:\Pathto\SetEfficiencyMode.exe" --no-console

:: Startup No Console Example
reg add "HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Run" /v "Set Efficiency Mode" /t REG_SZ /d "C:\Pathto\SetEfficiencyMode.exe --no-console" /f
```

