using UnityEngine;
using UnityEditor;
using System.Xml;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System;
using System.Text;

public class TreeNode
{
    public string name;
    public string value;
    public List<TreeNode> children = new List<TreeNode>();
}
public class ExportParticalTool : EditorWindow
{
    string myString = "Hello World";
    bool groupEnabled;
    bool myBool = true;
    float myFloat = 1.23f;
    string goalPath = Application.dataPath + "/Particles";
    GameObject go;
    Dictionary<string, TreeNode> pathMap = new Dictionary<string,TreeNode>();
    // Add menu named "My Window" to the Window menu
    [MenuItem("Tool/ExportParticalTool")]
    static void Init()
    {
        // Get existing open window or if none, make a new one:
        ExportParticalTool window = (ExportParticalTool)EditorWindow.GetWindow(typeof(ExportParticalTool));
    }

    void OnGUI()
    {
        GUILayout.Label("Base Settings", EditorStyles.boldLabel);
        myString = EditorGUILayout.TextField("Text Field", myString);

        groupEnabled = EditorGUILayout.BeginToggleGroup("Optional Settings", groupEnabled);
        myBool = EditorGUILayout.Toggle("Toggle", myBool);
        myFloat = EditorGUILayout.Slider("Slider", myFloat, -3, 3);
        EditorGUILayout.TextField("Text Field", goalPath);
        EditorGUILayout.EndToggleGroup();

        EditorGUILayout.BeginHorizontal();
        go = (GameObject)EditorGUILayout.ObjectField(go, typeof(GameObject), true);
        EditorGUILayout.EndHorizontal();

        if (GUILayout.Button("export this"))
        {
            using (XmlTextWriter xmlWriter = new XmlTextWriter(goalPath + "/" + go.name + ".xml", null))
            {
                xmlWriter.Formatting = Formatting.Indented;
                xmlWriter.WriteStartDocument(false);
                ExportGameObject(go, xmlWriter);
                xmlWriter.WriteEndDocument();
            }
        }
    }
    void ExportGameObject(GameObject go, XmlTextWriter xmlWriter)
    {
        if(go != null)
        {
            xmlWriter.WriteStartElement(go.name.Replace(" ", ""));
            TreeNode posNode = new TreeNode();
            posNode.name = "Position";
            ExportVec3Data(posNode, go.transform.localPosition);
            ExportProperty(posNode, xmlWriter);
            TreeNode rotNode = new TreeNode();
            rotNode.name = "Rotation";
            ExportQuaternionData(rotNode, go.transform.localRotation);
            ExportProperty(rotNode, xmlWriter);
            TreeNode scaleNode = new TreeNode();
            scaleNode.name = "Scale";
            ExportVec3Data(scaleNode, go.transform.localScale);
            ExportProperty(scaleNode, xmlWriter);
            ExportParticleSystem(go, xmlWriter);
            for (int i = 0; i < go.transform.childCount; i++)
            {
                ExportGameObject(go.transform.GetChild(i).gameObject, xmlWriter);
            }
            xmlWriter.WriteEndElement();
        }
    }
    TreeNode GenNode(string strName, string strValue)
    {
        TreeNode ret = new TreeNode();
        ret.name = strName;
        ret.value = strValue;
        return ret;
    }
    void ExportVec3Data(TreeNode parent, Vector3 vec3)
    {
        parent.children.Add(GenNode("x", vec3.x.ToString()));
        parent.children.Add(GenNode("y", vec3.y.ToString()));
        parent.children.Add(GenNode("z", vec3.z.ToString()));
    }
	void ExportQuaternionData(TreeNode parent, Quaternion q)
	{
		parent.children.Add(GenNode("x", q.x.ToString()));
		parent.children.Add(GenNode("y", q.y.ToString()));
		parent.children.Add(GenNode("z", q.z.ToString()));
		parent.children.Add(GenNode("w", q.w.ToString()));
	}

    void ExportParticleSystem(GameObject go, XmlTextWriter xmlWriter)
    {
        if (go != null)
        {
            ParticleSystem ps = go.GetComponent<ParticleSystem>();
            if(ps != null)
            {
                pathMap.Clear();
                SerializedObject so = new SerializedObject(ps);
                SerializedProperty it = so.GetIterator();
                TreeNode root = new TreeNode();
                root.name = "ParticleSystem";
                pathMap.Add("ParticleSystem", root);
                while (it.Next(true))
                {
                    //Debug.Log(string.Concat("name:", it.displayName, " type:", it.type, " :", it.propertyType.ToString()));
                    string strPath = string.Concat("ParticleSystem.", it.propertyPath);
                    TreeNode node = GenNode(it.name, GetPropertyByString(it));
                    pathMap.Add(strPath, node);
                    TreeNode parent = FindParentNode(strPath);
                    if (parent != null)
                    {
                        parent.children.Add(node);
                    }
                }
                ParticleSystemRenderer renderer = ps.GetComponent<ParticleSystemRenderer>();
                SerializedObject sor = new SerializedObject(renderer);
                SerializedProperty itr = sor.GetIterator();
                TreeNode rendererModule = GenNode("RendererModule", "");
                root.children.Add(rendererModule);
                pathMap.Add("ParticleSystem.Renderer", rendererModule);
                while(itr.Next(true))
                {
                    string strPath = string.Concat("ParticleSystem.Renderer.", itr.propertyPath);
                    TreeNode node = GenNode(itr.name, GetPropertyByString(itr));
                    pathMap.Add(strPath, node);
                    TreeNode parent = FindParentNode(strPath);
                    if (parent != null)
                    {
                        parent.children.Add(node);
                    }
                }
                RemoveNodeFromRoot(rendererModule, "m_ObjectHideFlags");
                RemoveNodeFromRoot(rendererModule, "m_PrefabParentObject");
                RemoveNodeFromRoot(rendererModule, "m_PrefabInternal");
                RemoveNodeFromRoot(rendererModule, "m_GameObject");
                RemoveNodeFromRoot(rendererModule, "m_LightmapIndex");
                RemoveNodeFromRoot(rendererModule, "m_LightmapIndexDynamic");
                RemoveNodeFromRoot(rendererModule, "m_LightmapTilingOffset");
                RemoveNodeFromRoot(rendererModule, "m_LightmapTilingOffsetDynamic");
                RemoveNodeFromRoot(rendererModule, "m_SubsetIndices");
                RemoveNodeFromRoot(rendererModule, "m_SubsetIndices");
                RemoveNodeFromRoot(rendererModule, "m_Materials");
                RemoveNodeFromRoot(rendererModule, "m_StaticBatchRoot");
                RemoveNodeFromRoot(rendererModule, "m_ProbeAnchor");
                RemoveNodeFromRoot(rendererModule, "m_LightmapParameters");
                RemoveNodeFromRoot(rendererModule, "m_Mesh");
                RemoveNodeFromRoot(rendererModule, "m_Mesh1");
                RemoveNodeFromRoot(rendererModule, "m_Mesh2");
                RemoveNodeFromRoot(rendererModule, "m_Mesh3");
                if (renderer.sharedMaterial != null)
                {
                    rendererModule.children.Add(GenNode("RenderMode", renderer.renderMode.ToString()));
                    rendererModule.children.Add(GenNode("Texture", renderer.sharedMaterial.mainTexture.name.ToLower()));
                    rendererModule.children.Add(GenNode("Shader", renderer.sharedMaterial.shader.name));
                    TreeNode OffsetNode = GenNode("Offset", "");
                    OffsetNode.children.Add(GenNode("X", renderer.sharedMaterial.mainTextureOffset.x.ToString()));
                    OffsetNode.children.Add(GenNode("Y", renderer.sharedMaterial.mainTextureOffset.y.ToString()));
                    rendererModule.children.Add(OffsetNode);
                    TreeNode TilingNode = GenNode("Tiling", "");
                    TilingNode.children.Add(GenNode("X", renderer.sharedMaterial.mainTextureScale.x.ToString()));
                    TilingNode.children.Add(GenNode("Y", renderer.sharedMaterial.mainTextureScale.y.ToString()));
                    rendererModule.children.Add(TilingNode);
                    if (renderer.sharedMaterial.HasProperty("_TintColor"))
                    {
                        Color clr = renderer.sharedMaterial.GetColor("_TintColor");
                        Color32 clr32 = clr;
                        TreeNode ColorNode = GenNode("TintColor", clr.ToString());
                        ColorNode.children.Add(GenNode("rgba", string.Concat(clr32.a.ToString("X2"), clr32.b.ToString("X2"), clr32.g.ToString("X2"), clr32.r.ToString("X2"))));
                        rendererModule.children.Add(ColorNode);
                    }
                }

                //clear unused module
                for (int i = root.children.Count - 1; i >= 0; i--)
                {
                    if (root.children[i].name.Contains("Module"))
                    {
                        if (root.children[i].children[0].value.CompareTo("False") == 0)
                        {
                            root.children.Remove(root.children[i]);
                        }
                    }
                }
                RemoveNodeFromRoot(root, "m_PrefabParentObject");
                RemoveNodeFromRoot(root, "m_PrefabInternal");
                RemoveNodeFromRoot(root, "m_GameObject");
                ExportProperty(root, xmlWriter);
            }
        }
    }
    void RemoveNodeFromRoot(TreeNode root, string strNodeName)
    {
        for (int i = root.children.Count - 1; i >= 0; i--)
        {
            if (root.children[i].name.CompareTo(strNodeName) == 0)
            {
                root.children.Remove(root.children[i]);
                break;
            }
        }
    }

    void ExportProperty(TreeNode node, XmlTextWriter xmlWriter)
    {
        xmlWriter.WriteStartElement(node.name);
        xmlWriter.WriteString(node.value);
        for (int i = 0; i < node.children.Count; i++ )
        {
            ExportProperty(node.children[i], xmlWriter);
        }
        xmlWriter.WriteEndElement();
    }
    public UInt16 ReverseByte16(UInt16 value)
    {
        return (UInt16)((value & 0xFFU) << 8 | (value & 0xFF00U) >> 8);
    }
    public UInt32 ReverseByte32(UInt32 value)//convert ABGR to RGBA
    {
        return (UInt32)ReverseByte16((UInt16)(value & 0xFFFFU)) << 16 | (UInt32)ReverseByte16((UInt16)((value & 0xFFFF0000U) >> 16));
    }

    string GetPropertyByString(SerializedProperty it)
    {
        string strRet = "";
        switch(it.propertyType)
        {
            case SerializedPropertyType.Integer:
                {
                    string specifier = "";
                    if (it.type.CompareTo("uint") == 0)
                    {
                        specifier = "X8";
                    }
                    strRet = it.intValue.ToString(specifier);
                }
                break;
            case SerializedPropertyType.Boolean:
                strRet = it.boolValue.ToString();
                break;
            case SerializedPropertyType.Float:
                strRet = it.floatValue.ToString();
                break;
            case SerializedPropertyType.String:
                strRet = it.stringValue;
                break;
            case SerializedPropertyType.Color:
                strRet = it.colorValue.ToString();
                break;
            case SerializedPropertyType.Enum:
                strRet = it.enumNames[it.enumValueIndex];
                break;
            case SerializedPropertyType.Vector2:
                strRet = it.vector2Value.ToString();
                break;
            case SerializedPropertyType.Vector3:
                strRet = it.vector3Value.ToString();
                break;
            case SerializedPropertyType.Vector4:
                strRet = it.vector4Value.ToString();
                break;
            case SerializedPropertyType.Rect:
                strRet = it.rectValue.ToString();
                break;
            case SerializedPropertyType.ArraySize:
                strRet = it.intValue.ToString();
                break;
            case SerializedPropertyType.Bounds:
                strRet = it.boundsValue.ToString();
                break;
            case SerializedPropertyType.AnimationCurve:
            case SerializedPropertyType.Character:
            case SerializedPropertyType.ObjectReference:
            case SerializedPropertyType.LayerMask:
            default:
                //Debug.Log(String.Concat("unexpected type:", it.type));
                break;
        }
        return strRet;
    }

    TreeNode FindParentNode(string strPath)
    {
        TreeNode ret = null;
        string parentPath = strPath.Substring(0, strPath.LastIndexOf("."));
        pathMap.TryGetValue(parentPath, out ret);
        return ret;
    }
}

