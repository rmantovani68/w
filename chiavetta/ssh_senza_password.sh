ssh roberto@$1 mkdir -p .ssh
cat .ssh/id_rsa.pub |ssh roberto@$1 'cat >> .ssh/authorized_keys'
ssh roberto@$1 chmod 700 .ssh
ssh roberto@$1 chmod 640 .ssh/authorized_keys

