cp ./lib/libgphoto2.so ./lib/libgphoto2.so.6
cp ./lib/libgphoto2_port.so ./lib/libgphoto2_port.so.12
cp ./lib/libusb.so ./lib/libusb-0.1.so.4
cp ./lib/libusb-1.0.so ./lib/libusb-1.0.so.0

cp ./lib/libgphoto2.so.6 /lib
cp ./lib/libgphoto2_port.so.12 /lib
cp ./lib/libusb-0.1.so.4 /lib
cp ./lib/libusb-1.0.so.0 /lib
cp ./lib/camlibs/ptp2.so /lib
cp ./lib/libxml2.so.2 /lib

mkdir -p /home/song/arm_libgphoto2/install_libgphoto2/lib/libgphoto2_port/0.12.2
mkdir -p /home/song/arm_libgphoto2/install_libgphoto2/lib/libgphoto2/2.5.31

cp -r ./lib/camlibs/* /home/song/arm_libgphoto2/install_libgphoto2/lib/libgphoto2/2.5.31
cp -r ./lib/iolibs/* /home/song/arm_libgphoto2/install_libgphoto2/lib/libgphoto2_port/0.12.2