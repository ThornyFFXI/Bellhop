# Bellhop
Bellhop is a plugin to make managing your inventory easier.  It operates entirely on typed commands, and offers the ability to do large batch actions with a single command.

# WARNING
Bellhop has very powerful commands.  This can make things much quicker.  However, it can also cause unintended results.  Please use extreme caution when using commands such as drop, sell, etc.. particularly with wildcards.  If you type something along the lines of '/bh dropall \*', bellhop will honor the request and drop everything in your inventory.  Caution should be employed when using bellhop immediately after a maintenance, to ensure inventory structure shifts have not occurred.  I am not responsible for any mistakes or bugs resulting in the loss of items, though I have put every effort forward to ensure the plugin behaves exactly as described.

# Installation
1. Download the plugin release zip that matches your ashita installation's interface version from the releases area on the right side.
2. Extract the entire zip to your ashita folder(the folder containing ashita.dll and ashita-cli.exe). Everything will fall into place.
3. Load with '/load bellhop' or add the same line to your startup script. Bellhop is a plugin not an addon, so do not use '/addon load'.
Do not download the entire repository as a zip unless you plan to compile the plugin yourself. That will not provide the files you need.

### Item Parameters
Anywhere where a command uses the term 'item', you may input items several different ways.  You can use:<br>

 * An item ID.  This is specified as simply the number, for example: '/bh getall 4096'.  This would get all fire crystals.
 * An item name.  This is not case sensitive, for example: '/bh getall "Fire Crystal"'.  Remember that anything containing a space must be encompassed in quotation marks.
 * A wildcard string.  This is also not case sensitive, for example: '/bh getall \*Crystal'.  This can match multiple different items.  Use caution when performing operations with wildcards.
 * An item type.  This must be specified beforehand in your settings XML.  For example: '/bh getall TYPE:Crystal' would look for a predefined item type called Crystal in your settings XML and match any item listed there.

### Commands
All commands can be prefixed with **/bellhop** or **/bh**.<br>
Any parameter that includes a space must be enclosed in quotation marks(**"**).<br>
If you are coming from Ashita3 and the commands look unfamiliar, please note that you can use the LegacyCommands setting to allow the old commands to also work.<br>

**/bh bazaar [required: item] [required: price]**<br>
**/bh bazaarall [required: item] [required: price]**<br>
Bazaar matching items.  Bazaar will bazaar one stack.  You must have the bazaar pricing menu already open.

**/bh drop [required: item]**<br>
**/bh dropall [required: item]**<br>
Drop matching items.  Drop will drop one stack.

**/bh get [required: item] [optional: quantity] [optional: container]**<br>
**/bh gets [required: item] [optional: container]**<br>
**/bh getall [required: item] [optional: container]**<br>
Retrieve matching items.  Gets will retrieve up to a stack.  Get will default to one item unless quantity is specified.  Quantity can exceed the size of one stack.  If a container is specified, bellhop will only retrieve items from that container.

**/bh put [required: item] [optional: quantity] [optional: container]**<br>
**/bh puts [required: item] [optional: container]**<br>
**/bh putall [required: item] [optional: container]**<br>
Store matching items.  Puts will store up to a stack.  Put will default to one item unless quantity is specified.  Quantity can exceed the size of one stack.  If a container is specified, bellhop will only store items in that container.

**/bh sell [required: item] [optional: quantity]**<br>
**/bh sells [required: item]**<br>
**/bh sellall [required: item]**<br>
Sell matching items.  Sells will store up to a stack.  Sell will default to one item unless quantity is specified.  Quantity can exceed the size of one stack.  You must have spoken to a NPC to open their shop, and stayed within range of that NPC, to use this command.

**/bh buy [required: item] [optional: quantity]**<br>
**/bh buys [required: item]**<br>
**/bh buyall [required: item]**<br>
Buy matching items.  Buys will buy up to a stack.  Buy will default to one item unless quantity is specified.  Buyall will buy the lesser of how many you can hold in inventory and how many you can afford.  Quantity can exceed the size of one stack.  You must have spoken to a NPC to open their shop, and stayed within range of that NPC, to use this command.  This does not work on shops with limited stock.

**/bh sort [required: container]**<br>
Sort a container.  This is not the same as the ingame sort; it will combine stacks but will not rearrange the order.

**/bh tradeplayer [required: item] [optional: quantity] [optional: item2] [optional: quantity2] [optional: item3] [optional: quantity3] [repeating]**<br>
**/bh tradeplayerall [required: item] [optional: item2] [optional: item3] [repeating]**<br>
Trade matching items to a player.  Tradeplayer will fail if the exact amount of items specified is not available or cannot fit in trade.  Tradeplayerall will trade what it is able to regardless.  When using multiple items in tradeplayer, all except the last item MUST have quantity specified.  Quantity can exceed the size of one stack, but the sum of all items/quantities must still fit within 8 slots.  You must already have trade window open to use this command, and it will silently confirm the trade, so do not click OK after sending the command, even though it does not show you have confirmed or the trade will fail.

**/bh tradenpc [required: item] [optional: quantity] [optional: item2] [optional: quantity2] [optional: item3] [optional: quantity3] [repeating]**<br>
**/bh tradenpcall [required: item] [optional: item2] [optional: item3] [repeating]**<br>
Trade matching items to currently targeted NPC.  Tradenpc will fail if the exact amount of items specified is not available or cannot fit in trade.  Tradenpcall will trade what it is able to regardless.  When using multiple items in tradenpc, all except the last item MUST have quantity specified.  Quantity can exceed the size of one stack, but the sum of all items/quantities must still fit within 8 slots.

**/bh default [required: accept/deny/ignore]**<br>
Sets the default behavior for autotrade.  This is overridden by whitelist/blacklist.  Using this command will automatically update your current settings xml.

**/bh blacklist [required: add/remove/clear/print] [required: name]**<br>
Edits or displays the autotrade blacklist.  Name is required when using add or remove.  Using add, remove, or clear will automatically update your current settings xml.

**/bh whitelist [required: add/remove/clear/print] [required: name]**<br>
Edits or displays the autotrade whitelist.  Name is required when using add or remove.  Using add, remove, or clear will automatically update your current settings xml.

**/bh enablebags [required: add/remove/clear/print] [required: container]**<br>
Edits or displays the list of force enabled bags.  Container is required when using add or remove.  Using add, remove, or clear will automatically update your current settings xml.

**/bh disablebags [required: add/remove/clear/print] [required: container]**<br>
Edits or displays the list of force disabled bags.  Container is required when using add or remove.  Using add, remove, or clear will automatically update your current settings xml.

**/bh config nomadstorage [required: enable/disable]**<br>
Sets nomadstorage setting.  When enabled, bellhop commands will act on storage while at a nomad moogle.  Note that the legitimate game client does not allow this and it could be a ban risk.  Using this command will automatically update your current settings xml.

**/bh config ignorecraftskill [required: enable/disable]**<br>
Sets ignorecraftskill setting.  When enabled, buy commands will be able to purchase items you have not yet unlocked from the unlimited stock guild shops.  Note that the legitimate game client does not allow this and it could be a ban risk.  Using this command will automatically update your current settings xml.

**/bh config legacycommands [required: enable/disable]**<br>
Sets legacycommands setting.  When enabled, bellhop commands from Ashita3 will work.  Using this command will automatically update your current settings xml.

**/bh config shortoutput [required: enable/disable]**<br>
Sets shortoutput setting.  When enabled, large batch commands will be reduced to a single line of output.  Using this command will automatically update your current settings xml.

**/bh config retrydelay [required: #]**<br>
Sets retrydelay setting to a time, in milliseconds, between 1 and 9999.  This is the length of time Bellhop will wait for server to respond before re-sending a packet for autotrade.  Using this command will automatically update your current settings xml.

**/bh help [optional: command]**<br>
If a command is specified, displays the command's input structure and a brief explanation of how it works.  If no command is specified, displays a list of available commands.

### Configuration
Settings are stored in a configuration file, located at Ashita/config/bellhop/default.xml.  This file will be created on first load.  If you want character-specific settings, you must create another file in the same folder, named (Your Current Character Name).xml.  For example, Ashita/config/bellhop/Thorny.xml.  While most settings in the config file can be changed via commands, item types will have to be manually entered into the configuration file.  See XML Structure.html for more information.