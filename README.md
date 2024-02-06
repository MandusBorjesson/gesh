# GESH - Good Enough Settings Handler

Simple D-Bus settings handler

# Setting handling

All actors that contibute to the setting handling are described in greater
detail in [settings.md](src/setting/setting.md). The upcoming sections detail
an overview of the more common scenarios when interacting with Gesh.

## Reading a setting

```mermaid
---
title: Setting read operation
---
sequenceDiagram
    actor User
    participant DBus
    box Gesh
        participant DBusAdaptor
        participant SettingHandler
    end
    User->>DBus: Read Setting(s)
    DBus->>+DBusAdaptor: owl.gesh.setting.Get(...)
    DBusAdaptor->>SettingHandler: Settings
    loop For each setting
        break Setting does not exist
            SettingHandler-->>DBusAdaptor: Error response
        end
    end
    SettingHandler-->>DBusAdaptor: Ok response (Settings)
    DBusAdaptor-->>-DBus: Response
    DBus-->>User: Response
```

## Changing a setting

```mermaid
---
title: Setting write operation
---
sequenceDiagram
    actor User
    participant DBus
    box Gesh
        participant DBusAdaptor
        participant SettingHandler
        participant ISettingRule
    end
    User->>DBus: Update Setting(s)
    DBus->>+DBusAdaptor: owl.gesh.setting.Set(...)
    DBusAdaptor->>SettingHandler: Settings
    loop For each setting
        break Setting does not exist
            SettingHandler-->>DBusAdaptor: Error response
        end
        SettingHandler->>ISettingRule: Validate rule
        break Rule validation failed
            ISettingRule-->>DBusAdaptor: Error response
        end
    end
    SettingHandler-->>SettingHandler: Update settings
    SettingHandler-->>DBusAdaptor: Ok response
    DBusAdaptor-->>-DBus: Response
    DBus-->>User: Response
    SettingHandler->>DBusAdaptor: Changed settings
    DBusAdaptor->>DBus: Emit SettingsUpdated with changed settings
```

# D-Bus handling

The D-Bus handling is described in greater detail in
[dbus.md](src/dbus/dbus.md). D-Bus was chosen for the external interface since
it is commonly used in linux environments, but the design should allow any
interface to be used in its stead.
