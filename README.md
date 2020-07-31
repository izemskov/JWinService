# JWinService

JWinService is simple way to run your Java Spring Boot application as windows service. 
You must only copy JWinService.exe to folder with your Java application, write simple configuration file, and run command:

```
JWinService.exe -install
JWinService.exe -start
```

for install and start your Java application as service

## Configuration file

Configuration file is a simple ini file:

Example:

```
[Service]
ServiceName=MyServiceName
FindJavaInRegistry=false
JavaArguments=-jar myjavaapp.jar
```

### ServiceName

Short display name of the service, which can contain spaces and other characters.

### FindJavaInRegistry

For true finding Java path in registry. For false finding Java path in PATH environment variable.

### JavaArguments

Arguments for JVM

## Usage

```
JWinService.exe [-command]
```

* -install   - install the service to Service Control Manager
* -uninstall - uninstall the service
* -start     - start the service
* -stop      - stop the service
* -enable    - enable service
* -disable   - disable service
