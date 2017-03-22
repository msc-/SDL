
Simple DirectMedia Layer 
========================

This is a fork of the [SDL Mercurial repository](http://hg.libsdl.org/SDL) at
http://hg.libsdl.org/SDL.

Branch `upstream_master` reflects the most recent pull from that repo's
`master` branch. The tip at the time was
http://hg.libsdl.org/SDL/rev/e212b4d00d84.

See [below](pulling_from_mercurial) for instructions on pulling from
the [SDL Mercurial repository](http://hg.libsdl.org/SDL)

See the main SDL [README](docs/README.md) for more information about
SDL.

What is Different Here
----------------------

This fork contains 1 main change from SDL 2.0.5+:

1. A script to create pre-built SDL libraries for Android, as described in
   [SDL Bugzilla](https://bugzilla.libsdl.org/) issue [2839](https://bugzilla.libsdl.org/show_bug.cgi?id=2839)

These fixes are needed by the
[KTX project](https://github.com/KhronosGroup/KTX); this fork provides
the source it uses.

Fixes for [SDL Bugzilla](https://bugzilla.libsdl.org/) issues
[2570](https://bugzilla.libsdl.org/show_bug.cgi?id=2570) and
[3145](https://bugzilla.libsdl.org/show_bug.cgi?id=3145), which first
appeared in this fork, have now been merged upstream and, as a result, are still included here.

Pulling from Mercurial
----------------------

If you want to pull libSDL 2.0.4 from the upstream Mercurial repo,
you need to

* install [Python 2](https://www.python.org/downloads/).
* install [Mercurial](http://mercurial.selenic.com/)
* install `git-remote-hg`.

### Installing Mercurial

When installing Mercurial for Windows, choose the appropriate `*py2.7`
installer so that `git-remote-hg` can `import mercurial` from your
Python installation. 

### Installing `git-remote-hg`

`git-remote-hg` can be installed from this
[GitHub project](https://github.com/fingolfin/git-remote-hg).

:o: Note: this is a fork of the original project with fixes for compability with
Mercurial 3.2+.

The example installation commands below copy `git-remote-hg` to `/usr/local/bin`.
 
```bash
sudo curl -o /usr/local/bin/git-remote-hg https://raw.githubusercontent.com/fingolfin/git-remote-hg/master/git-remote-hg
sudo chmod +x /usr/local/bin/git-remote-hg
```

If using Git Bash or Git Shell on Windows, create a ~/bin directory, making
sure it is in your `$PATH`, and put it there instead of `/usr/local/bin`.

:bangbang: If using Git Bash, do not be tempted to use `/bin`. The file
will end up in
`%USERPROFILE%\AppData\Local\VirtualStore\Program Files (x86)\Git\bin` and
will not be visible to the Windows version of `python` which will be told
by `env` to run `%SystemDrive%\Program Files (x86)\Git\bin\git-remote-hg`.
The latter is the canonical location of `/bin`. I do not know if
Git Shell has a similar issue. :bangbang:

You need to ensure you have a `python2` command in some directory
in your `$PATH`.

On OS X, whether `python2` exists depends on which distribution of Python 2
you are using. If it does not exist, make a link in `/usr/local/bin` to
the `python` command.

On Windows, the standard Python installer does not create a `python2`
command. One solution is to make a copy of
`%SystemDrive%\Python27\python.exe` (the default install location), e.g.

```bash
cp $SYSTEMDRIVE/Python27/python.exe ~/bin
```

As a last resort you can edit `git-remote-hg` and change the
first line

```
- #!/usr/bin/env python2
+ #!/usr/bin/env python
```

### Pulling the Source

Once Mercurial and `git-remote-hg` are installed, add a remote
that points to the SDL Mercurial repo.

```bash
git remote add upstream hg::http://hg.libsdl.org/SDL
```
Then issue the following commands

```bash
git checkout upstream_master
git pull upstream master
```

{# vim: set ai ts=4 sts=4 sw=2 expandtab textwidth=75:}

