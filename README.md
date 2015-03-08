# passtool
Password tool to generate passwords, but also to manage the passwords for an entire server park.

Available functions:
- Generate random passwords
- Generate fixed passwords based on 2 words, this creates the following possibility:

If you have a server park of 1000 servers and you want to give them all a different password,
this was a very hard job in the past.  You needed a repository (excel mostly) or a password tool
to store all the different passwords.

This program gives you the possibility of creating a new password for every server based on its
hostname and 1 key that can be used for every server.

The tool will always recreate the same password if you supply the same 2 words.

For example:


# ./passtool server123 
Key: 
Retype Key: 
Wv_SF4aK
# ./passtool server123 
Key: 
Retype Key: 
Wv_SF4aK

the result is always the same for the same combination.  

It is possible to customize the outcome of the style of the key (what caracters are used, how long 
must the password be, ...).

It is also possible to customize the password generator, making it custom to your organisation.
This last feature is only available by changing the code and rebuilding the tool, but in the
future it will be possible to change the behavour using a unique key in the config file.
