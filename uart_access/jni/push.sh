#/bin/sh
adb push /home/zhr/android/mountfs/a31_4.4/android4.4/out/target/product/fiber-a31st/system/lib/libuart_access.so /system/lib
adb push /home/zhr/android/mountfs/a31_4.4/android4.4/out/target/product/fiber-a31st/system/bin/uart_test /system/bin
adb shell chmod 777 /system/bin/uart_test

adb push /home/zhr/android/mountfs/a31_4.4/android4.4/out/target/product/fiber-a31st/system/bin/uart_test_test /system/bin
adb shell chmod 777 /system/bin/uart_test_test
adb shell sync


