# Utility library for Return To The Roots

Can be easily build with CMake.

## Externals

It uses the following external libraries which are included as `git subtree`s:

- Boost.Nowide
  - Source repo can be added with `git remote add flamefire-nowide https://github.com/Flamefire/nowide.git`
  - Update source from remote: `git subtree pull --prefix external/nowide flamefire-nowide v<...> --squash`
  - Contribute back: `git subtree push --prefix=external/nowide <your-nowide-remote> <your-branch>`

To avoid adding git submodules for new libs add them as subtrees with:
`git subtree add --prefix external/<lib> <remote> <tag> --squash`
