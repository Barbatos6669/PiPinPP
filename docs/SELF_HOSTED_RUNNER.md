# Setting Up Your Raspberry Pi as a GitHub Actions Runner

This guide will help you set up your Raspberry Pi to run hardware tests automatically as part of CI.

## What This Does

Your Pi will:
- ✅ Wait for CI jobs from GitHub
- ✅ Run builds and tests with REAL GPIO hardware
- ✅ Report results back (green checkmark or red X)
- ✅ Only run on main branch (not every PR)

## Prerequisites

- Raspberry Pi connected to internet
- SSH access or physical access
- User account in `gpio` group (you already have this)

## Setup Steps

### 1. Get Your GitHub Token

Go to your repository settings:
```
https://github.com/Barbatos6669/PiPinPP/settings/actions/runners/new
```

Click **"New self-hosted runner"**
- Choose: **Linux**
- Architecture: **ARM64** (if Pi 4/5) or **ARM** (if older Pi)

GitHub will show you a token - **copy it**! You'll need it in step 3.

### 2. Download the Runner on Your Pi

SSH into your Pi and run:

```bash
# Create a folder for the runner
mkdir ~/actions-runner && cd ~/actions-runner

# Download the latest runner (ARM64 for Pi 4/5)
curl -o actions-runner-linux-arm64-2.311.0.tar.gz -L \
  https://github.com/actions/runner/releases/download/v2.311.0/actions-runner-linux-arm64-2.311.0.tar.gz

# Extract it
tar xzf ./actions-runner-linux-arm64-2.311.0.tar.gz
```

**Note**: If you have an older 32-bit Pi, use `actions-runner-linux-arm-2.311.0.tar.gz` instead.

### 3. Configure the Runner

```bash
# Configure with the token from step 1
./config.sh --url https://github.com/Barbatos6669/PiPinPP --token YOUR_TOKEN_HERE

# When prompted:
# - Runner name: raspberry-pi (or whatever you want)
# - Runner group: Default
# - Labels: self-hosted,Linux,ARM64 (press Enter to use defaults)
# - Work folder: _work (press Enter for default)
```

### 4. Install as a Service (Runs Automatically)

```bash
# Install the service (requires sudo)
sudo ./svc.sh install

# Start the service
sudo ./svc.sh start

# Check status
sudo ./svc.sh status
```

The runner is now installed and will:
- ✅ Start automatically on boot
- ✅ Restart if it crashes
- ✅ Run in the background

### 5. Verify It's Working

Go to:
```
https://github.com/Barbatos6669/PiPinPP/settings/actions/runners
```

You should see your Pi listed with a **green dot** (online).

## Testing the Setup

### Manual Trigger

1. Go to: `https://github.com/Barbatos6669/PiPinPP/actions`
2. Click **"CI"** workflow
3. Click **"Run workflow"** → **"Run workflow"**
4. Watch the "Hardware Test (Raspberry Pi)" job run on your Pi!

### Automatic Trigger

Push to `main` branch:
```bash
git push origin main
```

The hardware tests will run automatically.

## How It Works

The workflow has 3 jobs now:

### Job 1: Cloud Build (4 jobs)
- Ubuntu 22.04 Debug
- Ubuntu 22.04 Release
- Ubuntu 24.04 Debug
- Ubuntu 24.04 Release
- Runs on: GitHub's free x86_64 servers
- Tests: Compilation only (no hardware)
- Speed: ~5-7 minutes

### Job 2: Hardware Test (Raspberry Pi)
- Runs on: YOUR Raspberry Pi
- Tests: ALL tests including GPIO hardware
- Only runs on: `main` branch or manual trigger
- Speed: ~2-3 minutes

### Job 3: CodeQL Security Scan
- Runs on: GitHub servers
- Checks for: Security vulnerabilities
- Speed: ~10 minutes

## Troubleshooting

### Runner Not Showing as Online

```bash
# Check service status
sudo ./svc.sh status

# View logs
journalctl -u actions.runner.* -f
```

### Want to Stop the Runner

```bash
cd ~/actions-runner
sudo ./svc.sh stop
sudo ./svc.sh uninstall
```

### Want to Update the Runner

```bash
cd ~/actions-runner
sudo ./svc.sh stop
curl -o actions-runner-linux-arm64-2.311.0.tar.gz -L \
  https://github.com/actions/runner/releases/download/v2.311.0/actions-runner-linux-arm64-2.311.0.tar.gz
tar xzf ./actions-runner-linux-arm64-2.311.0.tar.gz
sudo ./svc.sh start
```

## Security Notes

- ✅ Runner only processes jobs from YOUR repository
- ✅ Only runs on `main` branch (not random PRs)
- ✅ You can see all jobs in GitHub Actions tab
- ⚠️ Don't run on public PRs from untrusted users (current config is safe)

## Benefits

1. **Real Hardware Validation**: Tests actually use GPIO pins
2. **Faster**: No QEMU emulation slowdown
3. **Free**: Uses hardware you already own
4. **Accurate**: ARM architecture, real timing, real devices

## What Gets Tested on Your Pi

✅ **basic_test** - GPIO pin control (opens /dev/gpiochip0)
✅ **thread_safety_test** - Concurrent GPIO access
✅ **timing_test** - Precise microsecond timing
✅ **exception_test** - Error handling
✅ **All examples** - Compile verification

All of these use your actual GPIO hardware!

---

Need help? Open an issue at: https://github.com/Barbatos6669/PiPinPP/issues
