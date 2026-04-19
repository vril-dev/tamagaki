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

## License

GPL-3.0
