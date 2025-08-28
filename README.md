# Set Efficiency Mode
Set Efficiency Mode is a lightweight C++ console application that lets you enable EcoQos/Efficiency Mode for any application. Users can easily add apps via a simple configuration file and run it no console!

<img width="1649" height="920" alt="image" src="https://github.com/user-attachments/assets/f4d99932-2b5c-4339-a8f5-5bbe3427f067" />

![GitHub Release Downloads](https://img.shields.io/github/downloads/QuakedK/Set-Efficiency-Mode/total)

# #1 Usage

> [!NOTE]
> Efficiency Mode (EcoQoS) is only available on Windows 11. Windows 10 does not support Efficiency Mode, even though Task Manager may show a leaf icon for suspended UWP apps.

1. Download [Microsoft Visual C++ Redistributable](https://aka.ms/vs/17/release/vc_redist.x64.exe)
2. Download the [Set Efficiency Mode Zip](https://github.com/QuakedK/Set-Efficiency-Mode/releases/download/EfficiencyMode/SetEfficiencyMode.zip).
3. Right-click & extract it.
4. Edit and add the apps of your choosing to the Config.txt.
5. Once downloaded, run SetEfficiencyMode.exe as admin!

# #2 Config Format
```
Discord.exe
Spotify.exe
```
# #3 Command-line arguments
Set Efficiency Mode can be ran with a No Console flag to silently launch itself!
```
:: No Conole Flag
--no-console

:: CMD No Console Launch
"C:\Pathto\SetEfficiencyMode.exe" --no-console

:: Startup No Console Example
reg add "HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Run" /v "Set Efficiency Mode" /t REG_SZ /d "C:\Pathto\SetEfficiencyMode.exe --no-console" /f
```
# #4 Video Tutorial
https://github.com/user-attachments/assets/afdd52e7-7be8-4be5-ba33-7d972f3a314c

# #5 Looking for help!
This program was obviously created with ChatGPT as I have little to no experince with C++, and I'm willingly to work with anyone to improve this program!
Unfortunately it seems no one had made a program like this so I had to be the first at a lightweight console app that did what I wanted it to!
Contact me here [Discord Server](https://discord.gg/B8EmFVkdFU)!


