# Running PiPinPP Without Sudo

By default, GPIO access on Raspberry Pi requires root privileges. This guide shows you how to run your PiPinPP programs as a regular user without `sudo`.

## 🎯 TL;DR - Quick Setup

```bash
# Add yourself to the gpio group
sudo usermod -a -G gpio $USER

# Log out and back in (or reboot)
# Then your programs work without sudo!
./my_program
```

---

## 🔐 Why Do I Need Permissions?

GPIO pins are hardware resources that could potentially damage your system if misused. By default, only the `root` user can access them. This is a security feature.

### **The Problem:**

```bash
./my_led_program
# Error: Failed to open GPIO chip: Permission denied
```

### **Bad Solution:** ❌
```bash
sudo ./my_led_program  # Works, but annoying and risky
```

### **Good Solution:** ✅
Set up proper permissions once, then run normally forever!

---

## ✅ Method 1: GPIO Group (Recommended)

This is the **easiest and most common** method.

### **Step 1: Add Your User to the GPIO Group**

```bash
sudo usermod -a -G gpio $USER
```

**What this does:**
- `-a` = append (don't remove other groups)
- `-G gpio` = add to the "gpio" group
- `$USER` = your current username

### **Step 2: Verify Group Membership**

```bash
groups $USER
```

You should see `gpio` in the list:
```
hobbyhacker : hobbyhacker adm dialout cdrom sudo audio video plugdev games users input render netdev gpio
```

### **Step 3: Log Out and Back In**

**Important:** Group changes don't take effect until you log out and back in!

```bash
# Option 1: Reboot (safest)
sudo reboot

# Option 2: Log out and back in
# (Click your username → Log Out → Log back in)

# Option 3: Start a new login shell (for testing)
su - $USER
```

### **Step 4: Test It!**

```bash
cd ~/Documents/PiPinPP/build/examples
./example_basic_led  # Should work without sudo!
```

---

## ✅ Method 2: Udev Rules (Advanced)

If the gpio group method doesn't work (older Raspberry Pi OS versions), use udev rules.

### **Step 1: Create Udev Rule File**

```bash
sudo nano /etc/udev/rules.d/99-gpio.rules
```

### **Step 2: Add This Content**

```
# Allow users in gpio group to access GPIO character devices
SUBSYSTEM=="gpio", KERNEL=="gpiochip*", GROUP="gpio", MODE="0660"

# Allow users in gpio group to access GPIO lines
SUBSYSTEM=="gpio", KERNEL=="gpio*", GROUP="gpio", MODE="0660"
```

**What this does:**
- Tells the system: "When a GPIO device appears, set its group to 'gpio' and permissions to 0660"
- `0660` means: owner can read/write, group can read/write, others have no access

### **Step 3: Reload Udev Rules**

```bash
sudo udevadm control --reload-rules
sudo udevadm trigger
```

### **Step 4: Verify Permissions**

```bash
ls -l /dev/gpiochip*
```

Should show:
```
crw-rw---- 1 root gpio 254, 0 Nov  1 10:30 /dev/gpiochip0
           ^    ^^^^
           |     |
        owner  group
```

### **Step 5: Test It!**

```bash
./example_basic_led  # Should work!
```

---

## ✅ Method 3: System-Wide Installation (For Libraries)

If you install PiPinPP system-wide, ensure permissions are set correctly:

```bash
# Install library
cd ~/Documents/PiPinPP/build
sudo make install

# Verify library permissions
ls -l /usr/local/lib/libpipinpp.a
# Should be readable by all users

# Your programs will work without sudo
g++ -o myprogram myprogram.cpp -lpipinpp -lgpiod
./myprogram  # Works!
```

---

## 🔍 Troubleshooting

### **Problem: "Permission denied" even after adding to gpio group**

**Solution:** You didn't log out and back in!
```bash
# Check if gpio group is active in current session
groups
# If gpio is missing, log out and back in or reboot
```

### **Problem: "gpio group doesn't exist"**

**Solution:** Create it manually (older systems):
```bash
sudo groupadd gpio
sudo usermod -a -G gpio $USER
# Then log out/in
```

### **Problem: "/dev/gpiochip0 doesn't exist"**

**Solution:** Your kernel might be too old. Check:
```bash
uname -r
# Should be 4.8+ for character device support

# Update system
sudo apt update && sudo apt upgrade
```

### **Problem: "Still getting permission denied"**

**Solution:** Check libgpiod access specifically:
```bash
# Test libgpiod directly
gpiodetect
# If this fails, you need to fix libgpiod permissions

# Check gpiochip permissions
ls -l /dev/gpiochip*
# Should show: crw-rw---- 1 root gpio ...

# If not, udev rules aren't working. Reboot and try again.
```

### **Problem: "Works in terminal but not from cron/systemd"**

**Solution:** Service users need explicit group membership:
```bash
# For systemd service, add Group= to service file
[Service]
User=pi
Group=gpio
ExecStart=/path/to/program
```

---

## 🧪 Testing Your Permissions

### **Quick Permission Test:**

```cpp
// test_permissions.cpp
#include <iostream>
#include <gpiod.h>

int main() {
    struct gpiod_chip *chip;
    chip = gpiod_chip_open_by_name("gpiochip0");
    
    if (!chip) {
        std::cerr << "Failed to open GPIO chip" << std::endl;
        std::cerr << "Permission denied? Run: sudo usermod -a -G gpio $USER" << std::endl;
        return 1;
    }
    
    std::cout << "Success! GPIO permissions are working." << std::endl;
    std::cout << "Chip name: " << gpiod_chip_name(chip) << std::endl;
    std::cout << "Chip label: " << gpiod_chip_label(chip) << std::endl;
    
    gpiod_chip_close(chip);
    return 0;
}
```

**Compile and test:**
```bash
g++ test_permissions.cpp -o test_permissions -lgpiod
./test_permissions  # Should succeed without sudo
```

---

## 🛡️ Security Considerations

### **Is This Safe?**

**Yes**, when done correctly:
- ✅ Users in `gpio` group can only access GPIO, not other system resources
- ✅ GPIO access is isolated and protected
- ✅ This is the **intended method** per Raspberry Pi documentation

### **Bad Practices to Avoid:**

❌ **Don't run everything as root:**
```bash
sudo su  # Don't do this!
```

❌ **Don't give 777 permissions:**
```bash
sudo chmod 777 /dev/gpiochip0  # NEVER do this!
```

❌ **Don't disable udev:**
```bash
# Don't bypass the security system
```

### **Best Practice:**

✅ Use gpio group for development  
✅ Use specific users/groups for production services  
✅ Test permissions before deploying  

---

## 📋 Summary Checklist

Setup for non-root GPIO access:

- [ ] Add user to gpio group: `sudo usermod -a -G gpio $USER`
- [ ] Log out and back in (or reboot)
- [ ] Verify group membership: `groups $USER`
- [ ] Test GPIO access: `gpiodetect` (should work without sudo)
- [ ] Run PiPinPP programs without sudo
- [ ] If problems, check `/dev/gpiochip0` permissions
- [ ] Create udev rules if needed (Method 2)

---

## 🚀 Quick Start for New Projects

```bash
# Initial setup (one time only)
sudo usermod -a -G gpio $USER
sudo reboot

# After reboot, everything just works!
cd ~/Documents/PiPinPP/build/examples
./example_arduino_style  # No sudo needed!
```

---

## 📚 Additional Resources

- [Raspberry Pi GPIO Documentation](https://www.raspberrypi.com/documentation/computers/raspberry-pi.html)
- [Linux GPIO Subsystem](https://www.kernel.org/doc/html/latest/driver-api/gpio/index.html)
- [libgpiod Documentation](https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git/about/)
- [Udev Rules Tutorial](https://wiki.archlinux.org/title/Udev)

---

## ❓ FAQ

**Q: Do I need to do this every time I create a program?**  
A: No! Once you're in the gpio group, all programs work without sudo.

**Q: Will this work after a reboot?**  
A: Yes! Group membership and udev rules persist across reboots.

**Q: Can I remove myself from the gpio group later?**  
A: Yes: `sudo deluser $USER gpio` then log out/in.

**Q: Does this work on other Linux systems?**  
A: The gpio group is Raspberry Pi OS specific, but the udev rules method works on any Linux system with libgpiod.

**Q: What about running at boot (systemd service)?**  
A: Add `Group=gpio` to your systemd service file under `[Service]`.

**Q: Can I give access to multiple users?**  
A: Yes! Add each user to the gpio group: `sudo usermod -a -G gpio otheruser`
