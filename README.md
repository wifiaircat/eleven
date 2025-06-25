### Team eleven
System Programming 2025 Team 11

# Virt_Manager

<img width="748" alt="image" src="https://github.com/user-attachments/assets/21012aef-ba32-4df4-a3e0-9863b400a1bb" />

---

## ? Included Files

```
main.c, mount.c, check_status.c, wait.c, umount.c, ...
```

---

## ? How to Use

You can type one of the following commands:

```
mount / wait / umount / exit
```

* Modify the device name and mount path in `mount.c` to suit your setup.

---

## ? `mount`

Safely executes `insmod` and `mount`.

### ? Case 1: Success

* Starts tracking whether you're actively using the device.
* Prevents monopolization by detecting inactivity (customizable).
* Runs `Virt_manager` automatically.
* Launches a background thread to monitor activity.

### ? Case 2: Failure

* If the device is in use, the current user is shown.
* You are prompted to either:

  * `wait`: Add yourself to a FIFO queue.
  * `no`: Exit the program.
* When the current user finishes, you¡¯ll be notified.

---

## ? `wait`

Use this command when you know someone else is using the device and you just want to wait without attempting to mount.

* Adds your name directly to the FIFO queue.
* You'll be notified when it's your turn.

---

## ? `umount`

Safely performs:

```bash
umount /dev/your_device
rmmod your_module
```

* If successful, notifies the next user in the waiting queue.

---

## ? `exit`

Simply exits the `Virt_manager` program safely.