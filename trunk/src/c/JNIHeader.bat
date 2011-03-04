cd bin
javah -jni eisbot.proxy.JNIBWAPI
javap -s eisbot.proxy.JNIBWAPI > JNIBWAPI
cd .. 
copy bin\eisbot_proxy_JNIBWAPI.h eisbot_proxy_JNIBWAPI.h
copy bin\JNIBWAPI JNIBWAPI
pause
