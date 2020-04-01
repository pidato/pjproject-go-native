# pjproject-go-native

Bundled with:
- pjproject 2.10
- openssl 1.1.1d
- opus 1.3.1
- bcg729 1.0.4
- Opencore AMR
- fvad

## pjproject-ext

### PiPort
- OnGetFrame
- OnPutFrame

### PiRecorder

Records with Opus and VAD (Voice Activity Detection) on a Worker thread pool

## build
```
make
```

## debug
```
make debug
```

## Linux Dev Setup
```
sudo apt-get install build-essential automake libtool cmake uuid-dev

sudo make install-deps
```