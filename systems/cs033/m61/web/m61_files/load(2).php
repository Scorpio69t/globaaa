var mediaWikiLoadStart=(new Date()).getTime(),mwPerformance=(window.performance&&performance.mark)?performance:{mark:function(){}};mwPerformance.mark('mwLoadStart');function isCompatible(str){var ua=str||navigator.userAgent;return!!('querySelector'in document&&'localStorage'in window&&'addEventListener'in window&&!(ua.match(/webOS\/1\.[0-4]/)||ua.match(/PlayStation/i)||ua.match(/SymbianOS|Series60|NetFront|Opera Mini|S40OviBrowser|MeeGo/)||(ua.match(/Glass/)&&ua.match(/Android/))));}(function(){var NORLQ,script;if(!isCompatible()){document.documentElement.className=document.documentElement.className.replace(/(^|\s)client-js(\s|$)/,'$1client-nojs$2');NORLQ=window.NORLQ||[];while(NORLQ.length){NORLQ.shift()();}window.NORLQ={push:function(fn){fn();}};window.RLQ={push:function(){}};return;}function startUp(){mw.config=new mw.Map(true);mw.loader.addSource({"local":"/wiki/load.php"});mw.loader.register([["site","GeRyRApE"],["noscript","IetDfOkz",[],"noscript"],["filepage","1M/cM3dn"],[
"user.groups","dgiYAJ9b",[],"user"],["user","cDkbMOG+",[],"user"],["user.cssprefs","GqV9IPpY",[],"private"],["user.defaults","jhutCpfu"],["user.options","C9rS/VRT",[6],"private"],["user.tokens","3Xd46G5M",[],"private"],["mediawiki.language.data","bHbxm6Jh",[174]],["mediawiki.skinning.elements","1Lz4lc/J"],["mediawiki.skinning.content","0ugUZiDY"],["mediawiki.skinning.interface","ffZDY1X7"],["mediawiki.skinning.content.parsoid","UJAl3oBB"],["mediawiki.skinning.content.externallinks","WCg0FQfo"],["jquery.accessKeyLabel","tDXgHyhg",[25,130]],["jquery.appear","Hm7U4a/Z"],["jquery.arrowSteps","ZnBA1HTQ"],["jquery.async","G2j3FioD"],["jquery.autoEllipsis","Z64aSygq",[37]],["jquery.badge","7DGFEc1F",[171]],["jquery.byteLength","BI9udCNL"],["jquery.byteLimit","SMIX6qCl",[21]],["jquery.checkboxShiftClick","eESbONuf"],["jquery.chosen","Rr+QmkXv"],["jquery.client","b8oOXu+G"],["jquery.color","bsNCRdRX",[27]],["jquery.colorUtil","4g9t0hCf"],["jquery.confirmable","qCUJPTjC",[175]],["jquery.cookie",
"A6eV/qbU"],["jquery.expandableField","e/Q3a+cB"],["jquery.farbtastic","qd7+hb44",[27]],["jquery.footHovzer","C/7Lh/a4"],["jquery.form","A6qKGqvT"],["jquery.fullscreen","uR7DXRWd"],["jquery.getAttrs","zF9oCWbQ"],["jquery.hidpi","xcXeC9sU"],["jquery.highlightText","VDa71F8G",[242,130]],["jquery.hoverIntent","GpodEzDi"],["jquery.i18n","U57vOw5Y",[173]],["jquery.localize","ux9HfzWT"],["jquery.makeCollapsible","3OPXHblY"],["jquery.mockjax","9dYbd41J"],["jquery.mw-jump","QlJh6u2P"],["jquery.mwExtension","8cB+wPvA"],["jquery.placeholder","IVoaeSkG"],["jquery.qunit","qsTMTJBl"],["jquery.qunit.completenessTest","Y1ey7ytn",[46]],["jquery.spinner","6nNdWkHo"],["jquery.jStorage","wQX3T84T",[92]],["jquery.suggestions","2v+jsaZN",[37]],["jquery.tabIndex","Mwab49zI"],["jquery.tablesorter","v9Oj/Tln",[242,130,176]],["jquery.textSelection","8edc/Vhw",[25]],["jquery.throttle-debounce","3UjyPCzI"],["jquery.xmldom","wIjz76P2"],["jquery.tipsy","QK1B2SbP"],["jquery.ui.core","itWDRo9N",[58],"jquery.ui"],[
"jquery.ui.core.styles","eny68DFc",[],"jquery.ui"],["jquery.ui.accordion","TQYAX3mv",[57,77],"jquery.ui"],["jquery.ui.autocomplete","hv20Nu1l",[66],"jquery.ui"],["jquery.ui.button","MJM0VWcs",[57,77],"jquery.ui"],["jquery.ui.datepicker","AmqrjKdF",[57],"jquery.ui"],["jquery.ui.dialog","Qx8TjaH5",[61,64,68,70],"jquery.ui"],["jquery.ui.draggable","vL13V5uU",[57,67],"jquery.ui"],["jquery.ui.droppable","BipPsKdB",[64],"jquery.ui"],["jquery.ui.menu","qb2aWCNg",[57,68,77],"jquery.ui"],["jquery.ui.mouse","a36d+XaS",[77],"jquery.ui"],["jquery.ui.position","Dh0BF8IF",[],"jquery.ui"],["jquery.ui.progressbar","ZqYY+btO",[57,77],"jquery.ui"],["jquery.ui.resizable","J08L/Iyr",[57,67],"jquery.ui"],["jquery.ui.selectable","WD6QevDZ",[57,67],"jquery.ui"],["jquery.ui.slider","8X6GDp4X",[57,67],"jquery.ui"],["jquery.ui.sortable","c4DBeNuw",[57,67],"jquery.ui"],["jquery.ui.spinner","MydWnDcu",[61],"jquery.ui"],["jquery.ui.tabs","9EqnYl4J",[57,77],"jquery.ui"],["jquery.ui.tooltip","8RhQH3gD",[57,68,77],
"jquery.ui"],["jquery.ui.widget","5Pw/ZFZk",[],"jquery.ui"],["jquery.effects.core","HX4zY8uE",[],"jquery.ui"],["jquery.effects.blind","VSywn6xo",[78],"jquery.ui"],["jquery.effects.bounce","26XQzC8f",[78],"jquery.ui"],["jquery.effects.clip","MUGjTDp/",[78],"jquery.ui"],["jquery.effects.drop","WHdG/Olf",[78],"jquery.ui"],["jquery.effects.explode","0qaPhUNm",[78],"jquery.ui"],["jquery.effects.fade","UiHjRyAP",[78],"jquery.ui"],["jquery.effects.fold","U+aG23Rk",[78],"jquery.ui"],["jquery.effects.highlight","dYAR8E+u",[78],"jquery.ui"],["jquery.effects.pulsate","ObbbUtEG",[78],"jquery.ui"],["jquery.effects.scale","mXZQs3Oc",[78],"jquery.ui"],["jquery.effects.shake","l6eaWT36",[78],"jquery.ui"],["jquery.effects.slide","hpsKG9YL",[78],"jquery.ui"],["jquery.effects.transfer","q37rvC/Q",[78],"jquery.ui"],["json","io5mnlKj",[],null,null,"return!!(window.JSON\u0026\u0026JSON.stringify\u0026\u0026JSON.parse);"],["moment","bqMxmnKf"],["mediawiki.apihelp","CIjC0KKI"],["mediawiki.template","8vyuG5wI"
],["mediawiki.template.mustache","rwzFM1IO",[95]],["mediawiki.template.regexp","/NLVMNs7",[95]],["mediawiki.apipretty","8oSRt1gi"],["mediawiki.api","FrAfUeZl",[147,8]],["mediawiki.api.category","DnBdW2Ym",[135,99]],["mediawiki.api.edit","H9gu3MXh",[135,99]],["mediawiki.api.login","JxmlrcF0",[99]],["mediawiki.api.options","fzZSNPKb",[99]],["mediawiki.api.parse","hXmR3WIc",[99]],["mediawiki.api.upload","j8kFDW7v",[242,92,101]],["mediawiki.api.user","U13n70OW",[99]],["mediawiki.api.watch","kIYDS8Bd",[99]],["mediawiki.api.messages","NdYeiCsv",[99]],["mediawiki.content.json","Edr805Cv"],["mediawiki.confirmCloseWindow","eThIbmzD"],["mediawiki.debug","Bf+6YmqZ",[32,56]],["mediawiki.debug.init","v3J4d6l2",[111]],["mediawiki.feedback","3+ORcH34",[135,126,250]],["mediawiki.feedlink","nNbcn/i8"],["mediawiki.filewarning","m+FLH3dz",[245]],["mediawiki.ForeignApi","Xx4JTT1K",[117]],["mediawiki.ForeignApi.core","sXa+3I8b",[99,243]],["mediawiki.helplink","X/AULem2"],["mediawiki.hidpi","8M/yFNmW",[36],
null,null,"return'srcset'in new Image();"],["mediawiki.hlist","/FU6PY2p"],["mediawiki.htmlform","WF+m1ZPs",[22,130]],["mediawiki.htmlform.styles","rwMHvfHN"],["mediawiki.htmlform.ooui.styles","CJCxzGU+"],["mediawiki.icon","lmBMU53a"],["mediawiki.inspect","VUPlagxY",[21,92,130]],["mediawiki.messagePoster","OUX+Angy",[116]],["mediawiki.messagePoster.wikitext","3WDaJI0C",[101,126]],["mediawiki.notification","fjUonqQx",[183]],["mediawiki.notify","EuhT69JT"],["mediawiki.RegExp","8v3dtO+Y"],["mediawiki.pager.tablePager","HU8skGuv"],["mediawiki.searchSuggest","JBKkacHW",[35,45,50,99]],["mediawiki.sectionAnchor","BgU83Njb"],["mediawiki.storage","o+P03FMv"],["mediawiki.Title","zzs71ePc",[21,147]],["mediawiki.Upload","teP/4Y0m",[105]],["mediawiki.ForeignUpload","6UR/oF1G",[116,136]],["mediawiki.ForeignStructuredUpload.config","JtmRymYi"],["mediawiki.ForeignStructuredUpload","VYwS98id",[138,137]],["mediawiki.Upload.Dialog","V83UePch",[141]],["mediawiki.Upload.BookletLayout","3RyXUYB8",[136,175,
145,240,93,250,256,257]],["mediawiki.ForeignStructuredUpload.BookletLayout","0g0jMnNb",[139,141,108,179,236,234]],["mediawiki.toc","nVG+C1az",[151]],["mediawiki.Uri","/vhPVtRm",[147,97]],["mediawiki.user","4BFcPNzx",[106,151,7]],["mediawiki.userSuggest","eaopELZ1",[50,99]],["mediawiki.util","3qNEEsd0",[15,129]],["mediawiki.viewport","zn/uGqoM"],["mediawiki.checkboxtoggle","K/bLgg/9"],["mediawiki.checkboxtoggle.styles","bE0FC3TX"],["mediawiki.cookie","CdfZXFad",[29]],["mediawiki.toolbar","5howZXf9"],["mediawiki.experiments","0qL0Y9p8"],["mediawiki.raggett","6oKbPUTs"],["mediawiki.action.edit","405duMo9",[22,53,156]],["mediawiki.action.edit.styles","JOm6Ryv7"],["mediawiki.action.edit.collapsibleFooter","YORfEAO9",[41,151,124]],["mediawiki.action.edit.preview","MgXvSj3R",[33,48,53,161,99,175]],["mediawiki.action.edit.stash","nTfeMqsV",[35,99]],["mediawiki.action.history","gTcX0R47"],["mediawiki.action.history.diff","Z+m5KWf6"],["mediawiki.action.view.dblClickEdit","DC4NNnhF",[183,7]],[
"mediawiki.action.view.metadata","zoLJtknm"],["mediawiki.action.view.categoryPage.styles","bFbehiuK"],["mediawiki.action.view.postEdit","AIZ3xlI0",[151,175,95]],["mediawiki.action.view.redirect","Hv9+E136",[25]],["mediawiki.action.view.redirectPage","VaLJSWfB"],["mediawiki.action.view.rightClickEdit","Waj1rk7w"],["mediawiki.action.edit.editWarning","6W52kODA",[53,110,175]],["mediawiki.action.view.filepage","cIqNi2RG"],["mediawiki.language","mN9fJvc3",[172,9]],["mediawiki.cldr","9ut8B43s",[173]],["mediawiki.libs.pluralruleparser","pFXHpqS0"],["mediawiki.language.init","XP02k3EE"],["mediawiki.jqueryMsg","3BFJOerX",[242,171,147,7]],["mediawiki.language.months","z0Vwpujq",[171]],["mediawiki.language.names","dVn8oVHH",[174]],["mediawiki.language.specialCharacters","v31HqVfX",[171]],["mediawiki.libs.jpegmeta","AaEKc77Y"],["mediawiki.page.gallery","1/52OqtR",[54,181]],["mediawiki.page.gallery.styles","VM49y6K7"],["mediawiki.page.ready","u6sB71i2",[15,23,41,43,45]],["mediawiki.page.startup",
"QkTECq/S",[147]],["mediawiki.page.patrol.ajax","yN9oJBEK",[48,135,99,183]],["mediawiki.page.watch.ajax","kjbbfgCM",[107,183]],["mediawiki.page.image.pagination","CH202ceg",[48,147]],["mediawiki.special","TcYAA3iS"],["mediawiki.special.apisandbox.styles","ap7bRNn8"],["mediawiki.special.apisandbox","+aQa7NIT",[99,175,187,235,244]],["mediawiki.special.block","xzq/T+Q+",[147]],["mediawiki.special.blocklist","CpYBWnn0"],["mediawiki.special.changeslist","xXNwz2Q7"],["mediawiki.special.changeslist.legend","ZsN1A3mJ"],["mediawiki.special.changeslist.legend.js","c/lAJIXD",[41,151]],["mediawiki.special.changeslist.enhanced","5WvRZIDW"],["mediawiki.special.changeslist.visitedstatus","neYpb6lg"],["mediawiki.special.comparepages.styles","33AQFhdK"],["mediawiki.special.edittags","B6scpxQd",[24]],["mediawiki.special.edittags.styles","74TVbm3Y"],["mediawiki.special.import","J92CrF/q"],["mediawiki.special.movePage","kNSzwKSc",[232]],["mediawiki.special.movePage.styles","CQI+xmVu"],[
"mediawiki.special.pageLanguage","faa5C6K4",[245]],["mediawiki.special.pagesWithProp","JDAMuXoC"],["mediawiki.special.preferences","CsS4gMdD",[110,171,128]],["mediawiki.special.preferences.styles","Ogg9Gjsb"],["mediawiki.special.recentchanges","87o5ZKMp",[187]],["mediawiki.special.search","EI/ifT1M",[238]],["mediawiki.special.undelete","uhOkMGju"],["mediawiki.special.upload","LEsHlxww",[48,135,99,110,175,179,95]],["mediawiki.special.userlogin.common.styles","GFMfJwro"],["mediawiki.special.userlogin.signup.styles","uSysGeBT"],["mediawiki.special.userlogin.login.styles","8mE2Q49n"],["mediawiki.special.userlogin.signup.js","P4EcbiyL",[54,99,175]],["mediawiki.special.unwatchedPages","pLu9ybJ1",[135,107]],["mediawiki.special.watchlist","mY0UvdFy"],["mediawiki.special.version","OLt/NAbP"],["mediawiki.legacy.config","C7LNBkU0"],["mediawiki.legacy.commonPrint","cdE2AIKk"],["mediawiki.legacy.protect","27pE3dMS",[22]],["mediawiki.legacy.shared","OvHMK9Ln"],["mediawiki.legacy.oldshared",
"N/f+3pfJ"],["mediawiki.legacy.wikibits","VHxx+zrd",[147]],["mediawiki.ui","7lFwoDGG"],["mediawiki.ui.checkbox","jiQY6/Ps"],["mediawiki.ui.radio","O61Jhvsq"],["mediawiki.ui.anchor","UahREglR"],["mediawiki.ui.button","xB36W/K5"],["mediawiki.ui.input","bQAXRYA9"],["mediawiki.ui.icon","tR769otW"],["mediawiki.ui.text","D5AG0B2t"],["mediawiki.widgets","Fru1MwJd",[19,22,135,99,233,248]],["mediawiki.widgets.styles","hld8gCMk"],["mediawiki.widgets.DateInputWidget","Y/scrFJp",[93,248]],["mediawiki.widgets.datetime","+e35Aqm6",[245]],["mediawiki.widgets.CategorySelector","PWN5tl60",[116,135,248]],["mediawiki.widgets.UserInputWidget","en6pbfK+",[248]],["mediawiki.widgets.SearchInputWidget","Q2cnMb/6",[132,232]],["mediawiki.widgets.SearchInputWidget.styles","onIwHOuq"],["mediawiki.widgets.StashedFileWidget","/q3YCpN/",[245]],["es5-shim","qKzq703V",[],null,null,"return(function(){'use strict';return!this\u0026\u0026!!Function.prototype.bind;}());"],["dom-level2-shim","x9DGuYI8",[],null,null,
"return!!window.Node;"],["oojs","3vRGff64",[241,92]],["oojs-ui","DciK0AAo",[249,248,250]],["oojs-ui-core","IVKVdl5W",[171,243,246]],["oojs-ui-core.styles","D6XEChIq",[251,252,253],null,null,"return!!jQuery('meta[name=\"X-OOUI-PHP\"]').length;"],["oojs-ui.styles","HUIbEmss",[251,252,253],null,null,"return!!jQuery('meta[name=\"X-OOUI-PHP\"]').length;"],["oojs-ui-widgets","ufVWpIY0",[245]],["oojs-ui-toolbars","OAfJga7z",[245]],["oojs-ui-windows","uCbMuF3P",[245]],["oojs-ui.styles.icons","get7AMzI"],["oojs-ui.styles.indicators","pivKxVvr"],["oojs-ui.styles.textures","Zi7qyMYM"],["oojs-ui.styles.icons-accessibility","ZBTMK6RF"],["oojs-ui.styles.icons-alerts","Qy3iDlDy"],["oojs-ui.styles.icons-content","Z/lCLDYr"],["oojs-ui.styles.icons-editing-advanced","u7RgOAEk"],["oojs-ui.styles.icons-editing-core","FqtKSNe/"],["oojs-ui.styles.icons-editing-list","LZnCJvK1"],["oojs-ui.styles.icons-editing-styling","uOrwKdta"],["oojs-ui.styles.icons-interactions","EL+lZ3jX"],["oojs-ui.styles.icons-layout"
,"n2HT0zy7"],["oojs-ui.styles.icons-location","/WVZVQ9f"],["oojs-ui.styles.icons-media","m9UfCeRi"],["oojs-ui.styles.icons-moderation","USQ9PE13"],["oojs-ui.styles.icons-movement","yrk0l5Am"],["oojs-ui.styles.icons-user","TkllMRsf"],["oojs-ui.styles.icons-wikimedia","jyzGm7eB"],["skins.cologneblue","1rAv/5Rs"],["skins.modern","/bpdk1Li"],["skins.monobook.styles","BCDavCmE"],["skins.vector.styles","iLftlJsZ"],["skins.vector.styles.responsive","+0otnJDJ"],["skins.vector.js","h8f0kE5/",[51,54]],["ext.cite.styles","MrM9OaWj"],["ext.cite.a11y","UWtvfkmV"],["ext.cite.style","jCbJmCtt"],["ext.cite.visualEditor.core","GWJ8cujX",["ext.visualEditor.mwcore"]],["ext.cite.visualEditor.data","bc7ONLVh",["ext.visualEditor.base","ext.visualEditor.mediawiki"]],["ext.cite.visualEditor","75yH5SEG",[277,278,279,"ext.visualEditor.mwtransclusion"]],["ext.pygments","DXWf6QC0"],["ext.geshi.visualEditor","y8YVhAy4",["ext.visualEditor.mwcore"]]]);;mw.config.set({"wgLoadScript":"/wiki/load.php","debug":!1,
"skin":"vector","stylepath":"/wiki/skins","wgUrlProtocols":"bitcoin\\:|ftp\\:\\/\\/|ftps\\:\\/\\/|geo\\:|git\\:\\/\\/|gopher\\:\\/\\/|http\\:\\/\\/|https\\:\\/\\/|irc\\:\\/\\/|ircs\\:\\/\\/|magnet\\:|mailto\\:|mms\\:\\/\\/|news\\:|nntp\\:\\/\\/|redis\\:\\/\\/|sftp\\:\\/\\/|sip\\:|sips\\:|sms\\:|ssh\\:\\/\\/|svn\\:\\/\\/|tel\\:|telnet\\:\\/\\/|urn\\:|worldwind\\:\\/\\/|xmpp\\:|\\/\\/","wgArticlePath":"/wiki/$1","wgScriptPath":"/wiki","wgScriptExtension":".php","wgScript":"/wiki/index.php","wgSearchType":null,"wgVariantArticlePath":!1,"wgActionPaths":{},"wgServer":"https://cs61.seas.harvard.edu","wgServerName":"cs61.seas.harvard.edu","wgUserLanguage":"en","wgContentLanguage":"en","wgTranslateNumerals":!0,"wgVersion":"1.27.0","wgEnableAPI":!0,"wgEnableWriteAPI":!0,"wgMainPageTitle":"2016/Home","wgFormattedNamespaces":{"-2":"Media","-1":"Special","0":"","1":"Talk","2":"User","3":"User talk","4":"CS61","5":"CS61 talk","6":"File","7":"File talk","8":"MediaWiki","9":"MediaWiki talk",
"10":"Template","11":"Template talk","12":"Help","13":"Help talk","14":"Category","15":"Category talk"},"wgNamespaceIds":{"media":-2,"special":-1,"":0,"talk":1,"user":2,"user_talk":3,"cs61":4,"cs61_talk":5,"file":6,"file_talk":7,"mediawiki":8,"mediawiki_talk":9,"template":10,"template_talk":11,"help":12,"help_talk":13,"category":14,"category_talk":15,"image":6,"image_talk":7,"project":4,"project_talk":5},"wgContentNamespaces":[0],"wgSiteName":"CS61","wgDBname":"wikidb","wgExtraSignatureNamespaces":[],"wgAvailableSkins":{"cologneblue":"CologneBlue","modern":"Modern","monobook":"MonoBook","vector":"Vector","fallback":"Fallback","apioutput":"ApiOutput"},"wgExtensionAssetsPath":"/wiki/extensions","wgCookiePrefix":"wikidb","wgCookieDomain":"","wgCookiePath":"/","wgCookieExpiration":15552000,"wgResourceLoaderMaxQueryLength":2000,"wgCaseSensitiveNamespaces":[],"wgLegalTitleChars":" %!\"$&'()*,\\-./0-9:;=?@A-Z\\\\\\^_`a-z~+\\u0080-\\uFFFF","wgResourceLoaderStorageVersion":1,
"wgResourceLoaderStorageEnabled":!1,"wgResourceLoaderLegacyModules":[],"wgForeignUploadTargets":[],"wgEnableUploads":!0});var RLQ=window.RLQ||[];while(RLQ.length){RLQ.shift()();}window.RLQ={push:function(fn){fn();}};window.NORLQ={push:function(){}};}script=document.createElement('script');script.src="/wiki/load.php?debug=false&lang=en&modules=jquery%2Cmediawiki&only=scripts&skin=vector&version=ReikPTJt";script.onload=script.onreadystatechange=function(){if(!script.readyState||/loaded|complete/.test(script.readyState)){script.onload=script.onreadystatechange=null;script=null;startUp();}};document.getElementsByTagName('head')[0].appendChild(script);}());