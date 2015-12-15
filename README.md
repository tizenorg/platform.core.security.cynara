```
                                              -++-
                                             yyyyyy/
                                          .syyyyyyyys.
                                    .s/`  yyyyyyyyyyyy  `/s.
                                   `yyyo-yyyyyyyyyyyyyy oyyy.
                                   oyyy-syyyyyo++oyyyyys-yyyo
                                 ` syyy oyys/+sddy+/syyo yyys `
                                -hh//yyo s/ohddddddho s oyy//hh-
                                ydddh+/+./hddddddddddh+.+/+hdddh
                           .`  `ddddddh sdddddddddddddds hdddddd.  `.
                           hddy.dddddd/sdddddddddddddddds dddddd.sddd
                           yddd/sddddd.dddddddddddddddddd.dddddy dddh
                           +dddy-ddddd yddddddddddddddddy ddddd yddd+
                           `yddd+oddddy ddddddhhhhdddddd yddddo/dddh`
                            .yddd/oddds-+++++oooooo+++++-sdddo/dddy.
                              --++./s/+dddddddddddddddddd+/s/.++--
                                /ddhy++dddddddddddddddddd+/yhdd+
                                 `+ydd-hddddddddddddddddh-hdy+`
                                    `--` +ossyyyyyysso+ `--`



             -/++++/:`///// `////:-////.  -////-    ://-     :///++//:`      `///.
           -oso:--:ss .+ss:  /ss:``:ssss. `:ss:`   :ssss.    `/ss/-:oss.     /ssso`
          `sso`    --   +ss.-ss-   .ssoss- .ss.   .ss-oso     :ss-  :ss-    -ss-ss+
          -ss+           /ssss.    .ss-/ss/-ss.  `os/ -ss/    :sso+oso:    .ss: :ss:
          `sso            +ss-     .ss- :ssoss.  +ssssssss-   :ss:.+ss.    ossssssss.
           :sso:--:++    `+ss-`   `-ss:` -ssss.`:ss/` `-sss-``/ss:` +ss-`.+ss-` `:sso.
            `:/+o+/:`    +++++-   -++++:  .+++`/++++- .+++++-/++++: `/++:+++++. -+++++.
```

# Cynara
[Cynara][1] is fast, simple and safe policy-checker
service.
Functions of Cynara are:
* checking access for certain privileges
* holding policies
* simple, single function API - for checking policies
* thin client library - to make access control even more simple
* ability to use external agent
(in case of policies that can't be full processed in cynara and plugins)

## API Overview
Please refer to a wiki page on [Cynara's API][2].

## Documentation
Documentation is kept [here][1].

## Performance and comparison with existing solutions
There already exist some currently used access control solutions such as Polkit or Security-Server.
Analysis showed that there are some performance issues, lacks of functionality and unnecessary
complexity in existing policy-checkers. Below is showed simple comparison between Cynara and other
access control solutions, for more information about this topic look [here][3].

|                     Measure                     |  Polkit   | Security-Server | Cynara  |
|-------------------------------------------------|-----------|-----------------|---------|
| communication init - establishing communication |  12.37 ms | 0.08 ms         | 0.08 ms |
| communication - single request/response pair    |   3.35 ms | 0.19 ms         | 0.15 ms |
| single policy check                             | >14.45ms  | 9.5 ms          | 0.12 ms |

[1]: https://wiki.tizen.org/wiki/Security:Cynara
[2]: https://wiki.tizen.org/wiki/Security:Cynara:API
[3]: https://wiki.tizen.org/wiki/Security:Cynara:ComparisonWithOtherSolutions
