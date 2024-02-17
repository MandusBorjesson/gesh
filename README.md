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
    DBus->>+DBusAdaptor: owl.gesh.setting.Get(keys)
    loop For each key
        DBusAdaptor->>SettingHandler: Get(key)
        opt Setting does not exist
            SettingHandler-->>DBusAdaptor: Add to errors
        end
        opt No read access
            SettingHandler-->>DBusAdaptor: Add to errors
        end
        SettingHandler-->>DBusAdaptor: Add to settings
    end

    DBusAdaptor-->>-DBus: Response(settings, errors)
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
        participant DBusAdaptor1
        participant DBusAdaptor2
        participant SettingHandler
        participant ISettingRule
    end
    User->>DBus: Update Setting(s)
    DBus->>+DBusAdaptor2: owl.gesh.setting.Set(...)
    DBusAdaptor2->>SettingHandler: Set
    loop For each setting
        break Setting does not exist
            SettingHandler-->>DBusAdaptor2: Error response
        end
        SettingHandler->>ISettingRule: Validate rule
        break Rule validation failed
            ISettingRule-->>DBusAdaptor2: Error response
        end
    end
    SettingHandler-->>SettingHandler: Update settings

    SettingHandler->>DBusAdaptor1: Updated ∩ Readable(DBusAdaptor1)
    DBusAdaptor1->>DBus: Emit SettingsUpdated

    SettingHandler->>DBusAdaptor2: Updated ∩ Readable(DBusAdaptor2)
    DBusAdaptor2->>DBus: Emit SettingsUpdated

    SettingHandler-->>DBusAdaptor2: Ok response
    DBusAdaptor2-->>-DBus: Response
    DBus-->>User: Response
```

# D-Bus handling

The D-Bus handling is described in greater detail in
[dbus.md](src/dbus/dbus.md). D-Bus was chosen for the external interface since
it is commonly used in linux environments, but the design should allow any
interface to be used in its stead.
