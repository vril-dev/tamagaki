# Tamagaki

Lightweight Linux embedded security agent for IoT devices

## Overview

Tamagakiは、IoTデバイスを守るための軽量Linux組み込みセキュリティエージェントです。
デバイス自身に組み込まれ、通信を監視・制御する「玉垣」をコンセプトにしています。

- **L3〜L4**: eBPF/XDPとnftablesによる高速パケットフィルタリング・IDS
- **アウトバウンド監視**: IoTデバイスの異常な通信先を検知・遮断
- **インバウンド制御**: デバイス宛て通信のフィルタリング
- **軽量**: Buildrootベース、~30MBの最小rootfs
- **安全な更新**: A/B/CパーティションによるOTA、失敗時の自動フォールバック

## Architecture

```text
┌──────────────────────────┐
│       IoT Device         │
│                          │
│  App                     │
│   │                      │
│   ▼                      │
│  Tamagaki                │
│  L3-L4  eBPF/XDP/nft     │
│  IDS     monitoring      │
└──────────┬───────────────┘
           │
           ▼
   [LAN / Internet]
```

## Status

> 🚧 Work in progress

## Documentation

- [Design](docs/design.md)
- [Implementation Task](docs/design/tasks/001-tamagaki-linux-codex.md)

## Build Notes

The repository now contains a `br2-external` skeleton for Buildroot and a minimal C++/C userspace scaffold for `tamagaki`.

While the `tukuyomi-edge` release artifact is not yet published, `configs/tamagaki_defconfig` intentionally keeps `BR2_PACKAGE_TUKUYOMI_EDGE` disabled so Buildroot-side work can continue.

Host-side smoke build:

```bash
cmake -S . -B build
cmake --build build
./build/tamagaki --once --config board/tamagaki/rootfs-overlay/usr/share/tamagaki/tamagaki.yaml.example
```

Runtime control socket example:

```bash
./build/tamagaki \
  --config board/tamagaki/rootfs-overlay/usr/share/tamagaki/tamagaki.yaml.example \
  --socket /tmp/tamagaki.sock
```

```bash
python3 - <<'PY'
import socket

path = "/tmp/tamagaki.sock"
for command in [
    "health",
    "observe_mac aa:bb:cc:dd:ee:ff",
    "list_seen_macs",
    "block_mac aa:bb:cc:dd:ee:ff",
    "list_blocked_macs",
]:
    sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    sock.connect(path)
    sock.sendall((command + "\n").encode())
    print(sock.recv(65535).decode().strip())
    sock.close()
PY
```

## License

GPL-3.0
