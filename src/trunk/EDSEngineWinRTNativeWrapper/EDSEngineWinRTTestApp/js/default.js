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

function btnTest_onclick() {
    clear_results();
    var filename = "test_project.xml";
    var knowledge = new EDSEngineWinRT.EDSEngine(filename);
    //var knowledge = CreateKnowledgeBaseFromString(rules);
    if (knowledge.isOpen() == false) {
        write_result("FAILURE: Could not open rules xml file: " + filename);
        return false;
    }
    else {
        write_result("OK: File opened");
    }
    
    var cnt = knowledge.tableCount();
    if (cnt == 14) {
        write_result("OK: 14 Tables loaded");
    }
    else {
        write_result("FAILURE: Not all tables loaded: " + cnt.toString());
        return false;
    }

    var tableName = "testtable1";
    write_result("Loading attr output names for " + tableName);
    var allOutputNames = knowledge.getOutputAttrs(tableName);
    for (var i = 0; i < allOutputNames.length; i++) {
        write_result(allOutputNames[i]);
    }
    if (allOutputNames.length == 2 && allOutputNames[0] == "anotherOutput" && allOutputNames[1] == "outputAttr1") {
        write_result("OK");
    }
    else {
        write_result("FAILURE: Did not load all the output names");
        return false;
    }

    write_result("Loading attr input names for " + tableName);
    var allInputNames = knowledge.getInputAttrs(tableName);
    for (i = 0; i < allInputNames.length; i++) {
        write_result(allInputNames[i]);
    }
    if (allInputNames.length == 2 && allInputNames[0] == "inputAttr1" && allInputNames[1] == "inputAttr2") {
        write_result("OK");
    }
    else {
        write_result("FAILURE: Did not load all the input names");
        return false;
    }

    write_result("Loading dependency names for " + tableName);
    var allDepNames = knowledge.getInputDependencies(tableName);
    for (i = 0; i < allDepNames.length; i++) {
        write_result(allDepNames[i]);
    }
    if (allDepNames.length == 3 && allDepNames[0] == "inputAttr1" &&
        allDepNames[1] == "inputAttr2" &&
        allDepNames[2] == "outsideAttr1") {
        write_result("OK");
    }
    else {
        write_result("FAILURE: Did not load all the dependency names");
        return false;
    }

    var outputAttr = "outputAttr1";
    write_result("Getting all possible output values for " + outputAttr);
    var allOutputs = knowledge.getAllPossibleOutputs(tableName, outputAttr);
    for (var i = 0; i < allOutputs.length; i++) {
        write_result(allOutputs[i]);
    }
    if (allOutputs.length == 9 &&
        allOutputs[0] == "1" &&
        allOutputs[1] == "2" &&
        allOutputs[2] == "get(outsideAttr1) with concat" &&
        allOutputs[3] == "4" &&
        allOutputs[4] == "5" &&
        allOutputs[5] == "py(get(outsideAttr1) + 2)" &&
        allOutputs[6] == "js(get(outsideAttr1) + 2)" &&
        allOutputs[7] == "js(alterparameter())" &&
        allOutputs[8] == "py(alterparameter())") {
        write_result("OK");
    }
    else {
        write_result("FAILURE: Did not load all the possible output values");
        return false;
    }

    write_result("checking the table type for testtable1...");
    var isGetAll = knowledge.tableIsGetAll(tableName);
    if (isGetAll)
        write_result(tableName + " is GetAll as expected");
    else {
        write_result("FAILURE: table was not GetAll as expected");
        return false;
    }

    //testing table evaluation
    write_result("testing evaluation of testtable1 with inputAttr1 = 'C', get first only");
    knowledge.setInputValue("inputAttr1", "C");
    var results2 = knowledge.evaluateTable(tableName, false);
    if (ArraySize(results2) == 2 && results2["outputAttr1"][0] == "2") {
        write_result("OK: " + results2["outputAttr1"][0]);
    }
    else {
        write_result("FAILURE: Did not get proper eval result");
        alert(results2["outputAttr1"].length);
        alert(ArraySize(results2));
        alert("'" + results2["outputAttr1"][0] + "'");
        return false;
    }

    write_result("testing evaluation of testtable1 with inputAttr1 = 'C', get all");
    knowledge.setInputValue("inputAttr1", "C");
    var results = knowledge.evaluateTable(tableName, true);
    if (ArraySize(results) == 2 && results["outputAttr1"].length == 3 &&
        results["outputAttr1"][0] == "2" &&
        results["outputAttr1"][1] == "4" &&
        results["outputAttr1"][2] == "5") {
        write_result("OK: " + results["outputAttr1"][0] + "\n" +
        results["outputAttr1"][1] + "\n" +
        results["outputAttr1"][2]);
    }
    else {
        write_result("FAILURE: Did not get proper eval result");
        return false;
    }

    write_result("testing evaluation of testtable1 with inputAttr1 = 'C', inputAttr2 = 10, get all");
    knowledge.setInputValue("inputAttr1", "C");
    knowledge.setInputValue("inputAttr2", "10");
    var results3 = knowledge.evaluateTable(tableName, true);
    if (ArraySize(results3) == 2 && results3["outputAttr1"].length == 4 &&
        results3["outputAttr1"][0] == "2" &&
        results3["outputAttr1"][1] == " with concat" && //empty becuase we never fed it outsideAttr1
        results3["outputAttr1"][2] == "4" &&
        results3["outputAttr1"][3] == "5") {
        write_result("OK: " + results3["outputAttr1"][0] + "\n" +
        results3["outputAttr1"][1] + "\n" +
        results3["outputAttr1"][2] + "\n" +
        results3["outputAttr1"][3]);
    }
    else {
        write_result("FAILURE: Did not get proper eval result");
        return false;
    }

    write_result("testing evaluation (Javascript) of testtable1 with inputAttr1 = 'C', inputAttr2 = 78,  outsideAttr1 = 28, get all");
    knowledge.setInputValue("inputAttr1", "C");
    knowledge.setInputValue("inputAttr2", "78");
    knowledge.setInputValue("outsideAttr1", "28");
    var results4 = knowledge.evaluateTable(tableName, true);
    if (ArraySize(results4) == 2 && results4["outputAttr1"].length == 5 &&
        results4["outputAttr1"][0] == "2" &&
        results4["outputAttr1"][1] == "28 with concat" && //empty becuase we never fed it outsideAttr1
        results4["outputAttr1"][2] == "4" &&
        results4["outputAttr1"][3] == "5" &&
        results4["outputAttr1"][4] == "30") {
        write_result("OK: " + results4["outputAttr1"][0] + "\n" +
        results4["outputAttr1"][1] + "\n" +
        results4["outputAttr1"][2] + "\n" +
        results4["outputAttr1"][3] + "\n" +
        results4["outputAttr1"][4]);
    }
    else {
        write_result("FAILURE: Did not get proper eval result");
        return false;
    }

    write_result("testing evaluation (Javascript) with state parameter on testtable1 with inputAttr1 = 'TestParameterJS' and inputAttr2 = 'TestParameterJS'");
    knowledge.setInputValue("inputAttr1", "TestParameterJS");
    knowledge.setInputValue("inputAttr2", "TestParameterJS");
    var result9 = knowledge.evaluateTableWithParam(tableName, "outputAttr1", "PassedValue", knowledge.tableIsGetAll(tableName)); //JS is weakly typed watch the overloads
    var retParam = knowledge.getEvalParameter();
    write_result(ArraySize(result9));
    if (ArraySize(result9) == 4 && result9[3] == "eval ok" &&
        retParam == "PassedValue modified") {
        write_result("Javascript state parameter working");
    }
    else {
        write_result("FAILURE: Problem with Javascript state parameter: " + result9[3] + ":" + retParam);
        return false;
    }


    write_result("testing table chaining");
    knowledge.setInputValue("inputAttr1", "C");
    knowledge.setInputValue("inputAttr2", "78");
    knowledge.setInputValue("outsideAttr1", "28");
    tableName = "testtable2";
    var result5 = knowledge.evaluateTable(tableName, "out1", true);
    if (result5.length == results4["outputAttr1"].length &&
        result5[0] == "2" &&
        result5[1] == "28 with concat" && //empty becuase we never fed it outsideAttr1
        result5[2] == "4" &&
        result5[3] == "5" &&
        result5[4] == "30") {
        write_result("OK: " + result5[0] + "\n" +
        result5[1] + "\n" +
        result5[2] + "\n" +
        result5[3] + "\n" +
        result5[4]);
    }
    else {
        write_result("FAILURE: Did not get proper eval result on chain");
        return false;
    }

    knowledge.setDebugging(true, debugHandler);
    write_result("testing input get(), and debug");
    tableName = "testtable3";
    knowledge.setInputValue("someAttr", "3");
    var result6 = knowledge.evaluateTable(tableName, "outputAttr1", true);
    if (result6.length == 1 && result6[0] == "inputAttr2: 78 is greater than someAttr: 3") {
        write_result("OK: " + result6[0]);
    }
    else {
        write_result("FAILURE: Did not get proper eval result on get() test");
        return false;
    }
    knowledge.setDebugging(false);

    write_result("testing NULL conditions");
    knowledge.setInputValue("inputAttr1", "");
    knowledge.setInputValue("inputAttr2", "");
    var result7 = knowledge.evaluateTable("testtable4", "outputAttr1", true);
    if (result7.length == 4 && result7[2] == "both attrs are NULL") {
        write_result("OK: " + result7[0] + "\n" +
        result7[1] + "\n" +
        result7[2] + "\n" +
        result7[3]);
    }
    else {
        write_result("FAILURE: Did not get proper eval result on NULL test #1");
    }

    knowledge.setInputValue("inputAttr1", "blah");
    knowledge.setInputValue("c", "");
    result7 = knowledge.evaluateTable("testtable4", "outputAttr1", true);
    if (result7.length == 2 && result7[0] == "inputAttr2 is NULL") {
        write_result("OK: " + result7[0] + "\n" +
        result7[1]);
    }
    else {
        write_result("FAILURE: Did not get proper eval result on NULL test #2");
    }

    knowledge.setInputValue("inputAttr1", "blah");
    knowledge.setInputValue("c", "");
    result7 = knowledge.evaluateTable("testtable4", "outputAttr1", true);
    if (result7.length == 2 && result7[0] == "inputAttr2 is NULL") {
        write_result("OK: " + result7[0] + "\n" +
        result7[1]);
    }
    else {
        write_result("FAILURE: Did not get proper eval result on NULL test");
    }

    write_result("testing exclusing evaluation");
    knowledge.setInputValue("inputAttr1", "A");
    var result8 = knowledge.evaluateTable("exclusion", "outputAttr1", true);
    if (ArraySize(result8) == 4 && result8[0] == "not X or Y" &&
        result8[1] == "not X" &&
        result8[2] == "not Y" &&
        result8[3] == "fallout") {
        write_result("OK: " + result8[0] + "\n" +
        result8[1] + "\n" +
        result8[2] + "\n" +
        result8[3]);
    }
    else {
        write_result("FAILURE: Did not get proper eval result on exclusion test");
    }

    write_result("allowing a match: Y");
    knowledge.setInputValue("inputAttr1", "Y");
    result8 = knowledge.evaluateTable("exclusion", "outputAttr1", true);
    if (ArraySize(result8) == 4 && result8[0] == "is X or Y" &&
        result8[1] == "not X" &&
        result8[2] == "is Y" &&
        result8[3] == "fallout") {
        write_result("OK: " + result8[0] + "\n" +
       result8[1] + "\n" +
       result8[2] + "\n" +
       result8[3]);
    }
    else {
        write_result("FAILURE: Did not get proper eval result on exclusion test #2");
    }

    write_result("testing translation of: A");
    var localeValue = knowledge.localize("A", "en-US");
    var reverse = knowledge.deLocalize(localeValue);
    write_result(localeValue + ":" + reverse);
    if (localeValue == "A trans" && reverse == "A")
        write_result("OK");
    else
        write_result("FAILURE: translation failed");

    write_result("testing reverse evaluation of ReverseTest table");
    knowledge.setInputValue("OutColor", "green");
    var result9 = knowledge.reverseEvaluateTable("ReverseTest", true);
    if (ArraySize(result9) == 2 && result9["Color1"][0] == "blue" &&
        result9["Color2"][0] == "yellow") {
        write_result("OK: " + result9["Color1"][0] + ", " + result9["Color2"][0]);
    }
    else {
        write_result("FAILURE: reverse evaluation failed");
    }

    return false;
}

//no alert available
function alert(str)
{
    write_result(str);
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
