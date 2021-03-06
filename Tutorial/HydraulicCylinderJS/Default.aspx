﻿<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml">
<head id="Head1" runat="server">
    <title>Hydraulic Cylinder Configurator</title>    
    <script src="ajaxslt/util.js" type="text/javascript"></script>
    <script src="ajaxslt/xmltoken.js" type="text/javascript"></script>
    <script src="ajaxslt/dom.js" type="text/javascript"></script>
    <script src="ajaxslt/xpath.js" type="text/javascript"></script>
    <script src="ajaxslt/xslt.js" type="text/javascript"></script>
    <script src="KnowledgeBase.js" type="text/javascript"></script>    
    <script src="ROMNode.js" type="text/javascript"></script>
    <script type="text/javascript">
        var m_rootNode = null;
        var m_engine = null;
        var bLoadingItems = false;

        function ReportError(err) {
            var vDebug = "";
            for (var prop in err) {
                vDebug += "property: " + prop + " value: [" + err[prop] + "]\n";
            }
            vDebug += "toString(): " + " value: [" + err.toString() + "]";
            status.rawValue = vDebug;
            alert(vDebug);
        }

        function Initialize() {
            SetupApplication();
        }

        function SetupApplication() {
            try {
                m_rootNode = new ROMNode("HydraulicCylinder");
                if (!m_rootNode.LoadRules("HydraulicCylinderRules.xml")) {
                    alert("Error loading rules file");
                    return;
                }

                m_rootNode.SetRulesDebugHandler(DebugCallback);
                m_engine = new LinearEngine(m_rootNode, "HydraulicCylinderDictionary");
                m_engine.EvaluateAll();

                UpdateControls();
            }
            catch (err) {
                ReportError(err);
            }
        }

        function DebugCallback(msg) {
            var ctrlDebug = document.getElementById("debug");
            var filterCtrl = document.getElementById("filter");
            var tableName = filterCtrl.value;
            if (tableName.length > 0) {
                var newMsg = "";
                var indexStart = msg.indexOf("<TableEval name=\"" + tableName);
                if (indexStart >= 0) {
                    var indexEnd = msg.indexOf("</TableEval>", indexStart);
                    indexEnd += 12;
                    newMsg = msg.substr(indexStart, indexEnd);
                }
                msg = newMsg;
            }
            ctrlDebug.value += msg;
        }

        function ClearDebug() {
            var ctrlDebug = document.getElementById("debug");
            ctrlDebug.value = "";
        }

        function UpdateControls() {
            bLoadingItems = true;

            var allAttrs = m_engine.GetAllDictionaryAttrs();
            for (var key in allAttrs)
                SetControlUI(allAttrs[key]);
            bLoadingItems = false;
            UpdateCatalog();
        }

        function UpdateCatalog() {
            //catalog number is the concat of all the chars returned from the CatalogNumber table evaluation
            var allChars = m_rootNode.EvaluateTableForAttr("CatalogNumber", "Code");
            var Catnum = "";
            for (var i = 0; i < allChars.length; i++)
                Catnum += allChars[i];

            var catLabel = document.getElementById("Catalog");
            if (catLabel != null) {
                while (catLabel.childNodes.length)
                    catLabel.removeChild(catLabel.childNodes[0]);

                var textElement = document.createTextNode(Catnum);
                catLabel.appendChild(textElement);
            }
        }

        function SetControlUI(attr) {
            var ctrl = document.getElementById(attr.Name);
            var label = document.getElementById("lb" + attr.Name);

            if (ctrl != null) {
                switch (ctrl.type) {
                    case "select-one": //dropdown
                        //alert(attr.Name + " " + attr.AvailableValues.length + ":" + m_rootNode.GetAttribute(attr.Name, true));
                        ctrl.options.length = 0;
                        for (var cnt = 0; cnt < attr.AvailableValues.length; cnt++)
                            ctrl.options[cnt] = new Option(attr.AvailableValues[cnt], attr.AvailableValues[cnt], false, false);
                        if (ctrl.options.length > 0)
                            ctrl.selectedIndex = GetIndexOfItem(attr.AvailableValues, m_rootNode.GetAttribute(attr.Name, true));
                        break;
                    case "text": //textbox
                        ctrl.value = m_rootNode.GetAttribute(attr.Name, true);
                        break;
                }
                ctrl.disabled = !attr.Enabled;

                if (label != null) {
                    while (label.childNodes.length)
                        label.removeChild(label.childNodes[0]);

                    var textElement = document.createTextNode(attr.Description);
                    var font = document.createElement("font");

                    var makeBold = false;
                    if (attr.ChangedByUser == true)
                        makeBold = true;

                    if (attr.Valid == true) {
                        if (attr.Enabled == true)
                            font.style["color"] = "black";
                        else
                            font.style["color"] = "gray";
                    }
                    else
                        font.style["color"] = "red";

                    if (makeBold) {
                        var b = document.createElement("b");
                        label.appendChild(b);
                        b.appendChild(font);
                    }
                    else
                        label.appendChild(font);

                    font.appendChild(textElement);
                }
            }
        }

        function comboEvaluate(obj) {
            if (bLoadingItems)
                return;
            m_engine.EvaluateForAttribute(obj.id, obj.options[obj.selectedIndex].value, true);
            UpdateControls();
        }

        var txtValue = "";
        function txtTextChanged(obj) {
            if (bLoadingItems)
                return;
            if (txtValue != obj.value) {
                m_engine.EvaluateForAttribute(obj.id, obj.value, true);
                UpdateControls();
            }
        }

        function txtBeginEdit(obj) {
            txtValue = obj.value;
        }
        
        function SaveConfig() {
            document.forms['xmlform']['txtXML'].value = "";
            document.forms['xmlform']['txtXML'].value = m_rootNode.SaveXML(true);
        }
        
        function LoadConfig() {
            if (m_rootNode.LoadXML(document.forms['xmlform']['txtXML'].value))
            {
                if (m_engine != null)
                {
                    delete m_engine;
                    m_engine = null;
                }
                m_engine = new LinearEngine(m_rootNode, "HydraulicCylinderDictionary");
                m_engine.EvaluateAll();
                
                UpdateControls();        
            }               
        }
                
    </script>
</head>
<body oninit="Initialize()">
    <div>
        <input id="btnStart" title="Start Application" type="button" 
            value="Start Application" onclick="Initialize()"/></div>
    <table cols="3" style="width: 100%; height: 732px;">
        <tr>
            <td id="lbCylinderSeries">
                lbCylinderSeries</td>
            <td>
                <select id="CylinderSeries" onchange="comboEvaluate(this)">
                    <option></option>
                </select></td>
            <td>
                &nbsp;</td>
        </tr>
        <tr>
            <td id="lbMountingStyle">
                lbMountingStyle</td>
            <td>
                <select id="MountingStyle" onchange="comboEvaluate(this)">
                    <option></option>
                </select></td>
            <td>
                &nbsp;</td>
        </tr>
        <tr>
			<td>
                &nbsp;</td>
            <td id="lbBoreDiameter">
                lbBoreDiameter</td>
            <td id="lbRodDiameter">
                lbRodDiameter</td>            
        </tr>
        <tr>
			<td>
                &nbsp;</td>
            <td>
                <select id="BoreDiameter" onchange="comboEvaluate(this)">
                    <option></option>
                </select></td>
            <td>
                <select id="RodDiameter" onchange="comboEvaluate(this)">
                    <option></option>
                </select></td>            
        </tr>
        <tr>
            <td id="lbRodEndTypes">
                lbRodEndTypes</td>
            <td>
                <select id="RodEndTypes" onchange="comboEvaluate(this)">
                    <option></option>
                </select></td>
            <td>
                &nbsp;</td>
        </tr>
        <tr>
            <td id="lbSealingSystem">
                lbSealingSystem</td>
            <td>
                <select id="SealingSystem" onchange="comboEvaluate(this)">
                    <option></option>
                </select></td>
            <td>
                &nbsp;</td>
        </tr>
        <tr>
            <td id="lbPorts">
                lbPorts</td>
            <td>
                <select id="Ports" onchange="comboEvaluate(this)">
                    <option></option>
                </select></td>
            <td>
                &nbsp;</td>
        </tr>
        <tr>
			<td>
                &nbsp;</td>
            <td id="lbPortLocationHead">
                lbPortLocationHead</td>
            <td id="lbPortLocationCap">
                lbPortLocationCap</td>            
        </tr>
        <tr>
			<td>
                &nbsp;</td>
            <td>
                <select id="PortLocationHead" onchange="comboEvaluate(this)">
                    <option></option>
                </select></td>
            <td>
                <select id="PortLocationCap" onchange="comboEvaluate(this)">
                    <option></option>
                </select></td>            
        </tr>
        <tr>
			<td>
                Port Location</td>
            <td id="lbCushionHead">
                lbCushionHead</td>
            <td id="lbCushionCap">
                lbCushionCap</td>            
        </tr>
        <tr>
			<td>
                Cushion Locations</td>
            <td>
                <select id="CushionHead" onchange="comboEvaluate(this)">
                    <option></option>
                </select></td>
            <td>
                <select id="CushionCap" onchange="comboEvaluate(this)">
                    <option></option>
                </select></td>            
        </tr>
        <tr>
            <td id="lbCylinderStroke">
                lbCylinderStroke</td>
            <td>
                <input id="CylinderStroke" onfocus="txtBeginEdit(this)" onblur="txtTextChanged(this)"/>
            </td>
            <td>
                &nbsp;</td>
        </tr>
        <tr>
            <td id="lbAdditionalOption">
                lbAdditionalOption</td>
            <td>
                <select id="AdditionalOption" onchange="comboEvaluate(this)">
                    <option></option>
                </select></td>
            <td>
                &nbsp;</td>
        </tr>
        <tr>
			<td>
                &nbsp;</td>
            <td id="lbExtraRodProtection">
                lbExtraRodProtection</td>
            <td>
                Position</td>            
        </tr>
        <tr>
			<td>
                &nbsp;</td>
            <td>
                <input id="ExtraRodProtection" onfocus="txtBeginEdit(this)" onblur="txtTextChanged(this)"/>
            </td>
            <td>
				<table>
					<tr>
						<td id="lbOptionHead">
							lbOptionHead
						</td>
						<td id="lbOptionCap">
							lbOptionCap
						</td>
					</tr>
					<tr>
						<td>
							<select id="OptionHead" onchange="comboEvaluate(this)">
								<option></option>
							</select>
						</td>
						<td>
							<select id="OptionCap" onchange="comboEvaluate(this)">
								<option></option>
							</select>
						</td>
					</tr>
				</table>
            </td>            
        </tr>
        <tr>
			<td>
                &nbsp;</td>
			<td>
                Catalog#:</td>
			<td id = "Catalog">
                &nbsp;</td>
        </tr>
    </table>
    <div>
        <table>
            <tr>
                <td>
                    <input id="btnSaveConfig" title="Save Config XML" type="button" 
                        value="Save Config XML" onclick="SaveConfig()" align="left"/>
                </td>
                <td>
                    <input id="btnLoadXML" title="Load Config XML" type="button" 
                        value="Load Config XML" onclick="LoadConfig()" align="left"/>
                </td>
            </tr>
        </table>
        <form name="xmlform">
            <textarea id="txtXML" name="txtXML" cols=100 rows=50></textarea>
        </form>
    </div>
	<div>
		<input id="btnClear" title="Clear Debug" type="button" value="Clear Debug" onclick="ClearDebug()"/>
		Filter for table:
		<input type="text" id="filter"></input>
		<br />
		<textarea id="debug" cols=100 rows=50></textarea>
	</div>
    </body>
</html>
