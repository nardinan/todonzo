# TODONZO
### An amazing quick n' dirty reminder for Linux and Mac for people that have no time for GUIs (written in C)

Todonzo is the perfect companion for any busy programmer, constantly focused on a terminal writing code or typing commands. The system is relatively easy to use and fast to interface with any application you want (e.g., vim). The application uses libnotify to trigger notifications, but it can easily be interfaced with any notification routine you wish to use.

### Compile

Todonzo uses the standard CMake approach. Create a "build" folder, jump into it, and run your CMake command:

```console
git clone https://github.com/nardinan/todonzo
cd todonzo
mkdir build
cd build
cmake ..
```

Then, go with an easy-peasy make:

```console
make
sudo make install
```

Great! Todonzo is now installed. However, you may want him to check if some notification needs to be triggered now and then. To do that, feel free to use cron: we can ask our friend cron to check for pending notifications every minute:

```console
app_todonzo=`which todonzo` && user_environment=`env` && cat <<EOF | sudo tee /etc/cron.d/todonzo
$user_environment

* * * * * $USER . /home/$USER/.profile ; $app_todonzo -r
EOF
```

### Usage

{required parameter}

[optional parameter]

(syntax)

- todonzo {-a or --add} {title} [description] [+N(w or weeks | d or days | h or hours | m or mins)] [@(hour | hour:minute)]
- todonzo {-d or --delete} {UID}[,UID,UID,...]
- todonzo {-s or --show} [-x or --expired]
- todonzo {-r or --run}
- todonzo {-h or --help}

The easiest way to use Todonzo to push a new notification is:

```console
todonzo -a "Call the boss to validate the details of the USS Sulaco" +1day
```
> *Todonzo will notify you tomorrow at the same time.*

```console
todonzo -a "Check progresses in the main branch" +1week @10
```
> *Todonzo will notify you next week, same weekday, at 10 AM.*

```console
todonzo -a "Discuss with the team the MU-TH-UR 6000 project" @17
```
> *Todonzo will notify you today at 5 PM.*
