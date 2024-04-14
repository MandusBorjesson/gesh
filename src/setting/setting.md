# Settings

The `SettingHandler` is the central point for interacting with settings, the
diagram below details the relations between the `SettingHandler` and any other
setting-related classes.

# SettingHandler
The `SettingHandler` class is the main point of interaction for settings. It
handles setting access, updating, rule verification and much more.

# ISettingInitializer and ISettingStorage
Initializers and storages share a lot of similarities in that they both
provide settings to the [SettingHandler](#settinghandler). The main differences
are outlined in the sections below. 

## ISettingInitializer
The [SettingHandler](#settinghandler) only accept a single initializer. The
Initializer (any class derived from `ISettingInitializer`) tells the handler
what settings exist, and which [rules](#rule-handling) they must obey.

> [!NOTE]
> After the settings have been initialized, settings **can not** be added or
  removed, and rules **can not** be altered.  This is per design, and ensures
  that rules can never be invalidated, added or removed once they have been
  set up.

Settings that fail rule validation during initialization are ignored, although
this should be rare and is considered programmer error.

## ISettingStorage
Storages handle the bulk of setting sourcing. Is a storage honors setting
layers, they are also responsible for storing updates in a manner that allow
settings to be "reverted" to the value in a lower-priority layer.

### SettingStorageFile
Currently the only supported storage. Reads key-value pairs from a number of
plaintext files with the format `<key> <value>`. Keys values can be cleared by
using a special syntax `DELETE <key>`.

**Example syntax**
```
test.txt:

general/setting1 123
interface/ethernet/enable true
DELETE general/setting2
```

# Rule handling
All settings are required to have a rule. If a setting does not need to follow
any specific constraints, make it use a SettingRuleString since that is the
most lenient rule.
