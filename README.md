Lookup
======

Just a simple lookup table generation and search tool software.

Lookup tables
-------------

We consider the problem of finding back a value from its hash (in the
current implementation, MD5). Formally, we want to perform a preimage
attack. Since hash functions are designed to be resistant to preimage
attacks, we are often reduced to brute forcing: consider every single
possible value, hash it and match it against the target hash. This is
guaranteed to work but can take tremendeous amounts of time.

For feasible targets, we can speed up the process by storing the result
of all these hashes in a (sorted) table. That way, whenever a new hash
comes in, we simply have to look it up in the table. However, lookup
tables tend to be huge and impractical.

    C -> 12b2
    B -> 3e4f
    A -> 467f
    D -> 8801

Here, the hash 0e4f would be easily mapped to the value B.

Examples
--------

Generate a lookup table for 4 character words:

    $ ./ltgen 4 a4.lt

Crack a value with this table:

    % echo -n abcd | md5sum
    e2fc714c4727ee9395f324cd2e7f331f  -
    % ./ltcrack -x e2fc714c4727ee9395f324cd2e7f331f a4.lt 
    e2fc714c4727ee9395f324cd2e7f331f abcd

You can benchmark by cracking for random values:

    $ ./ltcrack -r 1000 a4.lt 
    1000 / 1000

**Note:** since a lookup table contains all the possible values, a test
should always validate completely

Licence
-------

This program is distributed under the GPL licence (see
[LICENCE.md](LICENCE.md) file). The credits for markdown formatting goes
to https://github.com/IQAndreas/markdown-licenses
