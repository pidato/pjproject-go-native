# pjproject-go-native

Bundled with:
- pjproject 2.10
- openssl 1.1.1d
- opus 1.3.1
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