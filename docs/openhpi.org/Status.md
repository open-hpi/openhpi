
# OpenHPI

### Schedule

![openhpi_schedule.png](http://openhpi.org/Status?action=AttachFile&do=get&target=openhpi_schedule.png "openhpi_schedule.png")  
[openhpi_schedule.svg](http://openhpi.org/Status?action=AttachFile&do=view&target=openhpi_schedule.svg)

Released

Current target

Future

### Release Status


| Release | Status Updates |
|--|--|
| **2.9.0** |Documentation updates|
| ^ | Client uniformity |
| **2.9.1** | Update alarm logic for sensor events (HPI B2) |
| ^ | Implement new HPI B2 return codes |
|**2.9.2** | Implement new Resource, Inventory, and Event Log APIs (HPI B2)|  
| ^ | User configuration of default domain| 
| ^ |Adjust return code if no plugin entrypoint (HPI B2) |
| **2.9.3** | Implement new FUMI APIs (HPI B2) |
| ^| Implement new DIMI APIs (HPI B2)|
  **2.10**|Conformance testing & Bug fixes |
### Bugs & Features

The following is a summary extract from the SourceForge tracker of  [bugs](http://sourceforge.net/tracker/?group_id=71730&atid=532251)  and  [features](http://sourceforge.net/tracker/?group_id=71730&atid=532254)  for OpenHPI categorized by release.

Open and Unassigned

<span style="color:red">some This is Red Bold.text</span>

Open and Assigned

Pending (Beta quality)

Closed and Fixed

**Features**

| > |**Simulator Plugin**  | <| <|
|--|--|--|--|
| 1977471|[Add Dimis and Fumis to simulator plugin](http://sourceforge.net/tracker/func=detail&aid=1977471&group_id=71730&atid=532254) |suntrupth yadav |Closed - Fixed |

#### 2.11.3
**Bugs**
| > |**IPMI Direct plugin** | < | <|
|--|--| --| --|
| 1930512 | [Invalid handling of ATCA Led Controls in Manual Mode](http://sourceforge.net/tracker/func=detail&aid=1930512&group_id=71730&atid=532251)|Shuah Khan| Closed - Fixed|

#### 2.13.3

**Features**
| > | **HP c-Class Plugin**| < | <|
|--|--|--|--|
| 2540102 | [Supporting the graceful shutdown for OA SOAP plugin](http://sourceforge.net/tracker/func=detail&aid=2540102&group_id=71730&atid=532254) |Raghavendra |Closed - Fixed|
| >|**OpenHPI Daemon**|<|<|
|1930512|[Invalid handling of ATCA Led Controls in Manual Mode](http://sourceforge.net/tracker/func=detail&aid=1930512&group_id=71730&atid=532251)|Shuah Khan|Closed - Fixed|

**Bugs**
| > |**HP ProLiant plugin**  |< |< |
|--|--|--|--|
| 2526494 |[sa HpiResourcePowerStateSet() fails with ilo2-ribcl Plugin](http://sourceforge.net/tracker/func=detail&aid=2526494&group_id=71730&atid=532251)  |Ric White | Closed - Fixed|
|>  | **HP c-Class Plugin**|< | <|
| 2567977 | [Me mory leak in event thread for not reachable OA](http://sourceforge.net/tracker/func=detail&aid=2567977&group_id=71730&atid=532251) |Bryan Sutula|Closed - Fixed |
| 2567974 | [Me mory leak in sensor event generation in OA SOAP plugin](http://sourceforge.net/tracker/func=detail&aid=2567974&group_id=71730&atid=532251) |Ric White|Closed - Fixed |
| 2543767 | [Potential race condition during startup.](http://sourceforge.net/tracker/func=detail&aid=2543767&group_id=71730&atid=532251) |Ric White | Closed - Fixed|
| 2529314 |[Hotswap event severity is wrong](http://sourceforge.net/tracker/func=detail&aid=2529314&group_id=71730&atid=532251)  | Shuah Khan|Closed - Fixed |
| 2527902 | [SEGFAULT encountered during the blade event processing](http://sourceforge.net/tracker/func=detail&aid=2527902&group_id=71730&atid=532251) |Bryan Sutula |Closed - Fixed|
|2515275  | [User's Guide needs to be updated](http://sourceforge.net/tracker/func=detail&aid=2515275&group_id=71730&atid=532251) | Shuah Khan|Closed - Fixed|
| 2496073 | [saHpiResourcePowerStateSet() fails with HP c-Class Plugin](http://sourceforge.net/tracker/func=detail&aid=2496073&group_id=71730&atid=532251) | Ric White| Closed - Fixed|
| > | **HPI Shell** | <|< |
| 2530818 | [incorrect severity when adding announcement to annunciator](http://sourceforge.net/tracker/func=detail&aid=2530818&group_id=71730&atid=532251) | Anton Pak|Closed - Fixed|
|  | **IPMI Direct plugin** | <| <|
| 2390475| [saHpiIdrAreaHeaderGet(), saHpiIdrFieldGet() dont handle type](http://sourceforge.net/tracker/func=detail&aid=2390475&group_id=71730&atid=532251) |Shuah Khan | Closed - Fixed|
| > | **OpenHPI Daemon** | <| <|
| 1804273 | [Hysteresis values are not validated correctly](http://sourceforge.net/tracker/func=detail&aid=1804273&group_id=71730&atid=532251) |Anton Pak |Closed - Fixed |
| > | **OpenHPI base library** | <|< |
| 2564289 | [gcc 4.4 - shadowed global variable](http://sourceforge.net/tracker/func=detail&aid=2564289&group_id=71730&atid=532251) |Nobody |Closed - Fixed |
| 2564015 | [gcc 4.4 - warning/error in safhpi.c](http://sourceforge.net/tracker/func=detail&aid=2564015&group_id=71730&atid=532251) |Nobody |Closed - Fixed |
| 2518985 |[SIGSEGV in tsensorget013](http://sourceforge.net/tracker/func=detail&aid=2518985&group_id=71730&atid=532251)  | Raghavendra|Closed - Fixed|
| > | **Test Suites** |< |< |
| 2568358 |[buffer overflows in sahpi_struct_utils_test](http://sourceforge.net/tracker/func=detail&aid=2568358&group_id=71730&atid=532251)  | Shuah Khan|Closed - Fixed |




