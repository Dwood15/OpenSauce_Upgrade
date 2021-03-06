/*
	BlamLib: .NET SDK for the Blam Engine

	See license\BlamLib\BlamLib for specific license information
*/
using System;
using System.Xml;
using System.Xml.Serialization;

namespace BlamLib.Render.COLLADA.Fx
{
	[SerializableAttribute()]
	[XmlTypeAttribute(AnonymousType = true)]
	public partial class ColladaLambert : ColladaElement
	{
		#region Fields
		ColladaObjectElement<ColladaCommonColorOrTextureType> _emission;
		ColladaObjectElement<ColladaCommonColorOrTextureType> _ambient;
		ColladaObjectElement<ColladaCommonColorOrTextureType> _diffuse;
		ColladaObjectElement<ColladaCommonColorOrTextureType> _reflective;
		ColladaObjectElement<ColladaCommonFloatOrParamType> _reflectivity;
		ColladaObjectElement<ColladaCommonColorOrTextureType> _transparent;
		ColladaObjectElement<ColladaCommonFloatOrParamType> _transparency;
		ColladaObjectElement<ColladaCommonFloatOrParamType> _indexOfRefraction;
		#endregion

		#region Children
		[XmlElement("emission")]
		public ColladaCommonColorOrTextureType Emission
		{ get { return _emission.Value; } set { _emission.Value = value; } }

		[XmlElement("ambient")]
		public ColladaCommonColorOrTextureType Ambient
		{ get { return _ambient.Value; } set { _ambient.Value = value; } }

		[XmlElement("diffuse")]
		public ColladaCommonColorOrTextureType Diffuse
		{ get { return _diffuse.Value; } set { _diffuse.Value = value; } }

		[XmlElement("reflective")]
		public ColladaCommonColorOrTextureType Reflective
		{ get { return _reflective.Value; } set { _reflective.Value = value; } }

		[XmlElement("reflectivity")]
		public ColladaCommonFloatOrParamType Reflectivity
		{ get { return _reflectivity.Value; } set { _reflectivity.Value = value; } }

		[XmlElement("transparent")]
		public ColladaCommonColorOrTextureType Transparent
		{ get { return _transparent.Value; } set { _transparent.Value = value; } }

		[XmlElement("transparency")]
		public ColladaCommonFloatOrParamType Transparency
		{ get { return _transparency.Value; } set { _transparency.Value = value; } }

		[XmlElement("index_of_refraction")]
		public ColladaCommonFloatOrParamType IndexOfRefraction
		{ get { return _indexOfRefraction.Value; } set { _indexOfRefraction.Value = value; } }
		#endregion

		public ColladaLambert() : base(Enums.ColladaElementType.Fx_Lambert)
		{
			Fields.Add(_emission = new ColladaObjectElement<ColladaCommonColorOrTextureType>());
			Fields.Add(_ambient = new ColladaObjectElement<ColladaCommonColorOrTextureType>());
			Fields.Add(_diffuse = new ColladaObjectElement<ColladaCommonColorOrTextureType>());
			Fields.Add(_reflective = new ColladaObjectElement<ColladaCommonColorOrTextureType>());
			Fields.Add(_reflectivity = new ColladaObjectElement<ColladaCommonFloatOrParamType>());
			Fields.Add(_transparent = new ColladaObjectElement<ColladaCommonColorOrTextureType>());
			Fields.Add(_transparency = new ColladaObjectElement<ColladaCommonFloatOrParamType>());
			Fields.Add(_indexOfRefraction = new ColladaObjectElement<ColladaCommonFloatOrParamType>());
		}
	}
}