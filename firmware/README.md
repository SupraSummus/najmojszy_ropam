# Firmware

Oryginalne pliki aktualizacji firmware'u urządzeń Ropam Elektronik,
w postaci, w jakiej producent udostępniał je do pobrania.
Prawa do nich należą do Ropam Elektronik.
Trzymamy je tu na potrzeby analizy interoperacyjności.
Jeśli właściciel praw sobie tego nie życzy, usuniemy je.

Format plików opisuje [`docs/firmware.md`](../docs/firmware.md).

| Plik | Urządzenie | Format |
|---|---|---|
| `optimagsm/optimagsm_v3.2_pl.flsh` | centrala OptimaGSM | Intel HEX + XOR |
| `optimagsm/optimagsm_v3.3_pl.flsh` | centrala OptimaGSM | Intel HEX + XOR |
| `optimagsm/optimagsm_v3.4_pl.flsh` | centrala OptimaGSM | Intel HEX + XOR |
| `tpr-optima/tpr_optima_v1.2_pl.flsh` | panel dotykowy TPR-1x/2x (OptimaGSM) | `rpmflash`, nierozpoznany |
| `tpr-optima/tpr_optima_v1.4_pl.flsh` | panel dotykowy TPR-1x/2x (OptimaGSM) | `rpmflash`, nierozpoznany |
| `apm-aero/…` | moduł radiowy APm-Aero | JSON + base64 z TI-TXT |

Sumy kontrolne: [`SHA256SUMS`](SHA256SUMS)
(sprawdzenie: `cd firmware && sha256sum -c SHA256SUMS`).
