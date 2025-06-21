sudo fio --directory=/home/aircat/nvmevirt/mnt \
  --direct=1 --ioengine=libaio --rw=randrw --bs=256k \
  --size=16g --time_based=1 --runtime=600 \
  --write_lat_log=hello --numjobs=1 \
  --disable_clat=1 --disable_slat=1 --log_avg_msec=1 --name write_test --rwmixread=0