FROM devkitpro/devkitppc:20251231
#RUN mkdir /opt/devkitpro/wut/include/wiiu
#RUN ln -s /opt/devkitpro/wut/include/coreinit /opt/devkitpro/wut/include/wiiu/os
RUN useradd -ms /bin/bash containeruser
run adduser containeruser sudo
RUN echo 'containeruser:lol' | chpasswd
USER containeruser
RUN echo ". /opt/devkitpro/wiiuvars.sh" > /home/containeruser/.bashrc

