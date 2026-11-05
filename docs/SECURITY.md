# Security and public release checklist

- The public firmware ships with non-personal Wi-Fi/AP defaults and no embedded printer SSH password. Configure credentials on the device. The v1.1.0 OTA build and printer authentication have been hardware tested; default starter configuration still requires setup.
- **AP starts enabled by default in the public starter**, unlike the owner’s configured build, to allow initial provisioning. Change the sample AP password before use or deployment. It is not a production secret.
- Set the printer root SSH password through **Settings → Printer SSH Password** before printer synchronization. The reader stores it in LittleFS `/config.ini` in plaintext; the settings request uses unauthenticated HTTP. Use only a trusted, isolated network and never commit or disclose the password.
- Normal OTA application updates retain the LittleFS configuration, including the stored printer password. **Migration from v1.0.1 or older No OTA partitions requires USB and can invalidate/format the old filesystem**; back up `material_database.json` and settings first.
- Rotate any previously exposed real credentials. Treat printer SSH access and the web UI as trusted-LAN-only; review authentication and input validation before exposing publicly or to untrusted networks.
- Printer sync/push are explicit browser actions. Keep a validated printer database backup, and do not change the known-good SCP/SSH logic casually.
- No secret scanner guarantees that all historical text is safe. Re-run `git grep` and a secret-scanning tool (e.g. gitleaks) over the complete repository history prior to making it public.
- Review any custom material files and documentation separately before publication.

- **Issue #2:** A contributor reported failures with literal backslashes in printer passwords; other K2 tests succeeded. Enter passwords literally in Settings (no C++ escaping). This compatibility report remains open pending independent reproduction on v1.1.0.
