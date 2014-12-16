// For an introduction to the Blank template, see the following documentation:
// http://go.microsoft.com/fwlink/?LinkId=232509
(function () {
    "use strict";

    WinJS.Binding.optimizeBindingReferences = true;

    var app = WinJS.Application;
    var activation = Windows.ApplicationModel.Activation;

    app.onactivated = function (args) {
        if (args.detail.kind === activation.ActivationKind.launch) {
            if (args.detail.previousExecutionState !== activation.ApplicationExecutionState.terminated) {
                // TODO: This application has been newly launched. Initialize
                // your application here.
            } else {
                // TODO: This application has been reactivated from suspension.
                // Restore application state here.
            }
            args.setPromise(WinJS.UI.processAll());
        }
    };

    app.oncheckpoint = function (args) {
        // TODO: This application is about to be suspended. Save any state
        // that needs to persist across suspensions here. You might use the
        // WinJS.Application.sessionState object, which is automatically
        // saved and restored across suspension. If you need to complete an
        // asynchronous operation before your application is suspended, call
        // args.setPromise().
    };

    app.start();
})();

function CreateChildNodes(rootNode) {
    write_result("Creating a child object");
    var childNode = new ROMWinRT.ROMNode("ChildObject");
    rootNode.addChildROMObject(childNode);
    childNode.setAttribute("childAttr", "some value of value");
    //setting a value on the Object Node
    childNode.setROMObjectValue("valueTest", "myValue");
    var childOfChild = new ROMWinRT.ROMNode("ChildObject2");
    childNode.addChildROMObject(childOfChild);
}

function btnTest2_onclick() {
    var start = 0; var elapsed = 0; var testROMTreeStart = 0; var testROMTreeEnd = 0; var testROMNodeStart = 0; var testROMNodeEnd = 0;
    var msg = new String();
    var iMax = 1000;
    try {
        clear_results();

        write_result("Test ROMNode Objects");
        write_result("Creating root node");
        var rootNode = new ROMWinRT.ROMNode("Test Application");
        write_result("Root ROMNode created");

        write_result("Setting some attributes");
        rootNode.setAttribute("inputAttr1", "some value of test1");
        rootNode.setAttribute("inputAttr2", "some value of test2");
        rootNode.setAttribute("inputAttr3", "some value of test3");
        rootNode.setAttribute("inputAttr3", "test3a", "some sub value of attr3_a");
        rootNode.setAttribute("inputAttr3", "test3b", "some sub value of attr3_b");
        write_result("Attrs Set");

        write_result("Testing what we have set");
        write_result("inputAttr1 = " + rootNode.getAttribute("inputAttr1", true));

        CreateChildNodes(rootNode);
        var findTest = rootNode.findAllObjectsByID("ChildObject", true);
        var findTestXPATH = rootNode.findObjects("//Object[@id='ChildObject']");
        var findTestXPATH2 = rootNode.findObjects("//Object[@id='ChildObject2']");
        if (ArraySize(findTest) == 1 && ArraySize(findTestXPATH) == 1 && ArraySize(findTestXPATH2) == 1 &&
            findTest[0].getROMGUID() == findTestXPATH[0].getROMGUID() &&
            findTestXPATH2[0].getROMObjectID() == "ChildObject2")
            write_result("OK");
        else
            write_result("FAILURE creating/obtaining child object");

        write_result("Dump current xml state");
        var result = rootNode.saveXML(true);
        write_result(result);

        write_result("Setting attrs to test eval, inputAttr1 = A, inputAttr2 = 1");
        rootNode.setAttribute("inputAttr1", "A");
        rootNode.setAttribute("inputAttr2", "1");

        write_result("New xml state");
        result = rootNode.saveXML(true);
        write_result(result);

        write_result("Getting attribute list on the base object");
        var allAttrs = rootNode.getAllAttributes();
        var attrKeyCnt = 0;
        for (var it in allAttrs) {
            var attrName = it;
            var kvp = allAttrs[attrName];
            write_result("Name: " + attrName);
            var valueKeyCnt = 0;
            for (var it_kvp in kvp) {
                write_result("Key: " + it_kvp + " Value: " + kvp[it_kvp]);
                valueKeyCnt++;
                if (valueKeyCnt >= kvp.size)
                    break;
            }
            attrKeyCnt++;
            if (attrKeyCnt >= allAttrs.size)
                break;
        }

        write_result("loading rules");
        //if (rootNode.LoadRulesFromString(rules))
        if (rootNode.loadRules("test_project.xml")) {
            write_result("...loaded");
            //deubg
            rootNode.debugDelegate = debugHandler;
            write_result("Evaluating table testtable1");
            var res = rootNode.evaluateTable("testtable1", "outputAttr1", true);
            for (var i = 0; i < res.length; i++) {
                write_result(res[i]);
            }
            write_result("Evaluation complete");

            write_result("Evaluating table testtable2: out1");
            var res2 = rootNode.evaluateTable("testtable2", "out1", true);
            for (var i = 0; i < res2.length; i++) {
                write_result(res2[i]);
            }
            write_result("Evaluation complete");


            write_result("Testing the LinearEngine object");
            var engine = new ROMWinRT.LinearEngine(rootNode, "Dictionary");

            write_result("Checking dictionary size");
            var attrs = engine.getAllDictionaryAttrs();
            if (ArraySize(attrs) == 6)
                write_result("size ok");
            else
                write_result("FAILURE loading dictionary:" + ArraySize(attrs));

            var order = engine.getEvalList();
            if (order != null && order.length == 6 &&
                order[0].name == "cDictAttr1" &&
                order[1].name == "dDictAttr2" &&
                order[2].name == "aDictAttr3" &&
                order[3].name == "bDictAttr4" &&
                order[4].name == "eDictAttr5" &&
                order[5].name == "eDictAttr6")
                write_result("Order OK");
            else
                write_result("FAILURE to assess the evaluation order:" + order.length + order[0].Name);

            var triggers = engine.getTriggers();
            if (ArraySize(triggers) == 3 &&
                triggers["aDictAttr3"].length == 2 &&
                triggers["aDictAttr3"][0] == "bDictAttr4" &&
                triggers["aDictAttr3"][1] == "eDictAttr5")
                write_result("Triggers OK");
            else
                write_result("FAILURE to assess the triggers");

            write_result("Testing evaluation");
            engine.evaluateAll();
            var attr1 = engine.getDictionaryAttr("cDictAttr1");
            var attr2 = engine.getDictionaryAttr("dDictAttr2");
            if (attr2.availableValues.length == 0 && attr2.possibleValues.length == 3 &&
                attr1.availableValues.length == 4)
                write_result("Default Eval OK");
            else
                write_result("FAILURE to initially evaluate an attribute");

            engine.evaluateForAttribute("cDictAttr1", attr1.availableValues[0], true);
            var val_pick1 = rootNode.getAttribute("dDictAttr2", true);
            var val_bool1 = rootNode.getAttribute("aDictAttr3", true);
            var val_multi1 = rootNode.getAttribute("bDictAttr4", true);
            var edit1 = rootNode.getAttribute("eDictAttr5", true);
            engine.evaluateForAttribute("cDictAttr1", attr1.availableValues[1], true);
            var val_pick2 = rootNode.getAttribute("dDictAttr2", true);
            var val_bool2 = rootNode.getAttribute("aDictAttr3", true);
            engine.evaluateForAttribute("cDictAttr1", attr1.availableValues[2], true);
            var val_bool3 = rootNode.getAttribute("aDictAttr3", true);
            var val_multi3 = rootNode.getAttribute("bDictAttr4", true);
            engine.evaluateForAttribute("eDictAttr5", "999", true);
            var edit4 = rootNode.getAttribute("eDictAttr5", true);
            if (val_pick1 == "ResultByOption1" && val_pick2 == "Result2" &&
                val_bool1 == "Y" && val_bool2 == "Y" && val_bool3 == "N" &&
                val_multi1 == "Selection2|Selection3" && val_multi3 == "Selection2" &&
                edit1 == "10" && edit4 == "50") {
                write_result("Evaluation of attributes ok");
            }
            else {
                write_result("FAILURE to evaluate an attribute");
                write_result("val_pick1:" + val_pick1 + " should be ResultByOption1");
                write_result("val_bool1:" + val_bool1 + " should be Y");
                write_result("val_multi1:" + val_multi1 + " should be Selection2|Selection3");
                write_result("edit1:" + edit1 + " should be 10");
                write_result("val_pick2:" + val_pick2 + " should be Result2");
                write_result("val_bool2:" + val_bool2 + " should be Y");
                write_result("val_bool3:" + val_bool3 + " should be N");
                write_result("val_multi3:" + val_multi3 + " should be Selection2");
                write_result("edit4:" + edit4 + " should be 50");
            }
        }
        else {
            write_result("Could not load rules");
        }

        start = 0;
        start = getTickCount();
        testROMTreeStart = start;
        write_result("Stress Testing...");
        write_result("Setting a bunch of values...");
        for (var i = 0; i < iMax; i++) {
            var attrToSet = "attr";
            attrToSet += i.toString();
            rootNode.setAttribute(attrToSet, i.toString());
        }
        write_result("Retrieving a bunch of values...");
        for (var i = 0; i < iMax; i++) {
            var attrToGet = "attr";
            attrToGet += i.toString();
            rootNode.getAttribute(attrToGet, true);
        }
        elapsed = getTickCount() - start;
        msg = "attribute test complete in ";
        msg += elapsed.toString();
        msg += "ms";
        write_result(msg);

        start = getTickCount();
        write_result("Create a large object hierarchy for query testing");
        for (i = 0; i < iMax; i++) {
            var objectToCreate = "SubLevel1Object";
            objectToCreate += i.toString();
            var newNode = new ROMWinRT.ROMNode(objectToCreate);
            rootNode.addChildROMObject(newNode);

            objectToCreate = "SubLevel2Object";
            objectToCreate += i.toString();
            var newNode2 = new ROMWinRT.ROMNode(objectToCreate);
            newNode.addChildROMObject(newNode2);
            newNode2.setAttribute("sumtester", "1");
            newNode2.setAttribute("sumtester2", "2");
            newNode2.setAttribute("sumtester3", "3");
            newNode2.setAttribute("testvalue", "value found");
        }
        elapsed = getTickCount() - start;
        msg = "objects created in ";
        msg += elapsed.toString();
        msg += "ms";
        write_result(msg);

        write_result("Performing queries");
        start = getTickCount();
        var queryTime = start;
        var xpath = "sum(//Attribute[@id='sumtester']/@value)";
        var xpathRes = rootNode.evaluateXPATH(xpath);
        elapsed = getTickCount() - start;
        write_result(xpath + " result: " + xpathRes);
        msg = "object query test complete in ";
        msg += elapsed.toString();
        msg += "ms";
        write_result(msg);

        start = getTickCount();
        var xpath2 = "sum(//Attribute[@id='sumtester2']/@value)";
        var xpathRes2 = rootNode.evaluateXPATH(xpath2);
        elapsed = getTickCount() - start;
        write_result(xpath2 + " result: " + xpathRes2);
        msg = "object query test complete in ";
        msg += elapsed.toString();
        msg += "ms";
        write_result(msg);

        write_result("altering the object state");
        rootNode.setAttribute("Change", "Y");

        start = getTickCount();
        var xpath3 = "sum(//Attribute[@id='sumtester3']/@value)";
        var xpathRes3 = rootNode.evaluateXPATH(xpath3);
        elapsed = getTickCount() - start;
        write_result(xpath3 + " result: " + xpathRes3);
        msg = "object query test complete in ";
        msg += elapsed.toString();
        msg += "ms";
        write_result(msg);

        start = getTickCount();
        var xpath4 = "//Object[@id='SubLevel2Object10']/Attribute[@id='testvalue']/@value";
        var xpathRes4 = rootNode.evaluateXPATH(xpath4);
        var finished = getTickCount();
        testROMTreeEnd = finished;
        elapsed = finished - start;
        write_result(xpath4 + " result: " + xpathRes4);
        msg = "object query test complete in ";
        msg += elapsed.toString();
        msg += "ms";
        write_result(msg);

        elapsed = finished - queryTime;
        msg = "All object query tests completed in ";
        msg += elapsed.toString();
        msg += "ms";
        write_result(msg);

        //all the destructors handle cleanup through IDispose
    }
    catch (err) {
        ReportError(err);
    }
}

//no alert available
function alert(str) {
    write_result(str);
}

function ReportError(err) {
    var vDebug = "";
    for (var prop in err) {
        vDebug += "property: " + prop + " value: [" + err[prop] + "]\n";
    }
    vDebug += "toString(): " + " value: [" + err.toString() + "]";
    status.rawValue = vDebug;
    alert(vDebug);
}

function debugHandler(str) {
    alert("DEBUGGING:\n" + str);
}

function write_result(res) {
    var textArea = document.getElementById("resultsText");
    textArea.value += res + "\n";
}

function clear_results() {
    var textArea = document.getElementById("resultsText");
    textArea.value = "";
}

function getTickCount() {
    var now = new Date();
    var ticks = now.getTime();
    return ticks;
}

function ArraySize(arr) //for associative array sizes, unlike regualar JS calls in IE there is a size
{
    var i = arr.size;
    return i;
}