# Library Documentation

``libcuteci`` is CuteCI's way of integrating with your application.

## Enums
### CuteCI::Flags

Values marked as *Default* are part of ``CUTECI_DEFAULT_FLAGS``

* ``CuteCI::DISABLE_LOGGING`` - Disables logging for CuteCI
* ``CuteCI::AUTO_RENDER`` - Automatically screenshot all new widgets added to the program (*Default*)
* ``CuteCI::AUTO_QUIT`` - Automatically quit the application after a fixed amount of time (*Default*)

## Defines
### CUTECI (*User defined*)
```c++
#define CUTECI
```
Enables CuteCI. Without it all macros will be ignored.

### CUTECI_INIT
```c++
[void] CUTECI_INIT(QObject* parent, int flags)
```

Initializes CuteCI and starts its background worker thread.

*Parameters:*  

* ``parent`` - Pointer to object on which the thread lifetime will depend. Once this object gets destroyed, CuteCI exits.
* ``flags`` - A combination of multiple CuteCI::Flags. If unsure use ``CUTECI_DEFAULT_FLAGS``.

### CUTECI_RENDER

```c++
[QWidget*] CUTECI_RENDER(QWidget* widget, const char* name)

```

Renders the specified ``widget`` and saves the screenshot using the ``name`` given.

*Parameters:* 

* ``widget`` - Pointer to widget to render.
* ``name`` - Filename of the resulting screenshot. *Note: You musn't to provide a file extension, if you do so anyway it will be ignored*.