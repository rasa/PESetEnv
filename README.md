# PESetEnv [![Flattr this][flatter_png]][flatter]

Set environment in [BartPE][].

## Usage

See plugin\z_pesetenv\z_pesetenv.htm for details

## Verify a Release

To verify a release, download the .zip, .sha256, and .asc files for the release 
(replacing PESetEnv-1.4-win32.zip with the release you are verifying):

````
$ wget https://github.com/rasa/PESetEnv/releases/download/v1.4/PESetEnv-1.4-win32.zip{,.sha256,.asc}
````

Next, check that sha256sum reports "OK":
````
$ sha256sum -c PESetEnv-1.4-win32.zip.sha256
PESetEnv-1.4-win32.zip: OK
````

Lastly, check that GPG reports "Good signature":

````
$ gpg --keyserver hkps.pool.sks-keyservers.net --recv-key 0x105a5225b6ab4b22
$ gpg --verify PESetEnv-1.4-win32.zip.asc PESetEnv-1.4-win32.zip
gpg:                using RSA key 0xFF914F74B4BB6EF3
gpg: Good signature from "Ross Smith II <ross@smithii.com>" [ultimate]
...
````

## Contributing

To contribute to this project, please see [CONTRIBUTING.md](CONTRIBUTING.md).

## Bugs

To view existing bugs, or report a new bug, please see [issues](../../issues).

## Changelog

To view the version history for this project, please see [CHANGELOG.md](CHANGELOG.md).

## License

This project is [MIT licensed](LICENSE).

## Contact

This project was created and is maintained by [Ross Smith II][] [![endorse][endorse_png]][endorse]

Feedback, suggestions, and enhancements are welcome.

[Ross Smith II]: mailto:ross@smithii.com "ross@smithii.com"
[flatter]: https://flattr.com/submit/auto?user_id=rasa&url=https%3A%2F%2Fgithub.com%2Frasa%2FPESetEnv
[flatter_png]: http://button.flattr.com/flattr-badge-large.png "Flattr this"
[endorse]: https://coderwall.com/rasa
[endorse_png]: https://api.coderwall.com/rasa/endorsecount.png "endorse"

[BartPE]: http://www.nu2.nu/pebuilder/