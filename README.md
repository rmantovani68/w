# wella

## build

### packages to install
#### fedora 39
```
sudo dnf install gtk2-devel-2.24.33-15.fc39.x86_64
sudo dnf install libgnome-devel-2.32.1-28.fc39.x86_64 -y
sudo dnf install libgnomeui-devel-2.24.5-30.fc39.x86_64 -y
```

#### ubuntu
```
sudo apt install build-essential -y
sudo apt install libgtk2.0-dev -y
sudo apt install libpq-dev -y
sudo apt install gnome-devel -y
```

## github
```
sudo apt install gh
gh auth login
```

## vscode
```
sudo apt install dirmngr ca-certificates software-properties-common apt-transport-https -y
curl -fSsL https://packages.microsoft.com/keys/microsoft.asc | sudo gpg --dearmor | sudo tee /usr/share/keyrings/vscode.gpg > /dev/null
echo deb [arch=amd64 signed-by=/usr/share/keyrings/vscode.gpg] https://packages.microsoft.com/repos/vscode stable main | sudo tee /etc/apt/sources.list.d/vscode.list
sudo apt update
sudo apt install code
```
