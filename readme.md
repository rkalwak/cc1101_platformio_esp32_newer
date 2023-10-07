Części: 
CC1101 868MHz - większość liczników używa 868MHz
ESP32 - co kto lubi

Można się wzorować na mojej implementacji : https://github.com/rkalwak/cc1101_platformio_esp32_newer
, aczkolwiek chylę czoła przed @SzczepanLeon który ogarnął komunikację z CC1101 i parsowanie protokołu licznika Apator 16-2, gdzie ja to zrozumiałem w inny sposób i walczyłem kilka tygodni... Tak więc kod w repozytorium to jest mix z jego repozytoriów z Supla, byle działało. Wiem, że będzie chciał to zrobić dobrze tak aby każdy licznik dało się odczytywać na wzór z wmbusmeters.com i może wtedy będzie można to zintegrować np. z GUI-Generic.

Kabelkologia, piny zgadzają się z ESP32 DevKit, ale dostosujcie sobie.
mosi = 23, 
miso = 19, 
clk = 18, 
cs = 5, 
gdo0 = 4, 
gdo2 = 2

Obrazkowo opis pinów na CC1101: https://community.home-assistant.io/t/izar-watermeter-wemos-d1-mini-cc1101/529079

UWAGA! Projekt używa VS Code z rozszerzeniem PlatformIO

Przed uruchomieniem kodu trzeba:
1. zmienić w pliku arduino_cc1101_wmbus.ino linijki zawierające poniższe zmienne, one są specyficzne dla każdego urządzenia.
Można je wygerować: 
https://www.supla.org/pl/arduino/get-guid 

  char GUID[SUPLA_GUID_SIZE] = {...};
https://www.supla.org/arduino/get-authkey
  char AUTHKEY[SUPLA_AUTHKEY_SIZE] = {...};

2. stworzyc w głównym katalogu plik secrets.ini o zawartości jak poniżej, oczywiście wpisując swoje dane do Supli
[code]
[env:esp32]
build_flags = 
    -D emailVariable='"supla email"'
    -D serverVariable='"supla serwer"'
    -D wifiSSIDString='"ssid"'
    -D wifiPasswordString='"password"'
[/code] 

3. W pliku platformio.ini (albo przez PlatformIO) dostosować uzywaną płytkę i wersję frameworka Arduino, użyłem starszej bo nie umiałem skompilować tego na nowszej... 
[code]
platform = espressif32@^6.0.0
platform_packages =
    framework-arduinoespressif32 @ https://github.com/espressif/arduino-esp32.git#2.0.9
    ;https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json @^2.0.11
board = esp32dev
framework = arduino
monitor_speed = 115200
[/code]

Powinno się zbudować a po wgraniu na ESP32 w cloud powinien się pojawić Licznik impulsów.
W cloud należy sobie ustawić w polu: Impulsy 1000000/ m³ (małe oszustwo w celu przeliczenia m3 na litry na ESP32 i potem powrotnie na m3 gdyż kanał licznika impulsów akceptuje tylko liczby całkowite), jak wartości się nie mieszczą można dołożyć zero i tak samo w kodzie.

Jeśli odczytujesz wiecej niz jeden licznik możesz użyć metody "extractMeterId" w celu pobrania z wiaodmosci ID licznika - logikę zostawiam Tobie ;)