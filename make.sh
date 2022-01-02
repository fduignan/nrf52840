west build -b nrf52840dongle_nrf52840 $1 
nrfutil pkg generate --hw-version 52 --sd-req=0x00 --application build/zephyr/zephyr.hex --application-version 1 zephyr.zip
nrfutil dfu usb-serial -pkg zephyr.zip -p /dev/ttyACM0
