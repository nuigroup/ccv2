/*

# IniFile

IniFile is a single C# file that offers reading from and writing to INI files.
You can drop it straight into your project without adding any DLL references.

## Basic usage

### Reading an INI file

```csharp
// [foo]
// bar=1
// baz=qux
// [foo2]
// bar=2
		
var iniFile = new IniFile("foo.ini");
iniFile.Section("foo").Get("bar"); // "1"
iniFile.Section("foo").Get("baz"); // "qux"
		
foreach (var section in iniFile.Sections())
	Console.WriteLine(section.Name); // foo
								     // foo2
```

### Writing an INI file	

```csharp
var iniFile = new IniFile();
	
iniFile.Section("foo").Comment = "This is foo";
iniFile.Section("foo").Set("bar", "1");
iniFile.Section("foo").Set("baz", "qux", comment: "baz is qux");

iniFile.Section("foo2").Set("bar", "2");

iniFile.Save("foo.ini");

// ;This is foo
// [foo]
// bar=1
// ;baz is qux
// baz=qux
//
// [foo2]
// bar=2

iniFile.Section("foo").RemoveProperty("bar"); // or iniFile.Section("foo").Set("bar", null);
iniFile.RemoveSection("foo2");
iniFile.Save("foo.ini");

// ;This is foo
// [foo]
// ;baz is qux
// baz=qux
```
*/


using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace Utilities
{
    public class IniProperty
    {
        /// <summary>
        /// Property name (key).
        /// </summary>
        public string Name { get; set; }

        /// <summary>
        /// Property value.
        /// </summary>
        public string Value { get; set; }

        /// <summary>
        /// Set the comment to display above this property.
        /// </summary>
        public string Comment { get; set; }
    }

    public class IniSection : IEnumerable<IniProperty>
    {
        private readonly IDictionary<string, IniProperty> _properties;

        public IniSection(string name)
        {
            Name = name;
            _properties = new Dictionary<string, IniProperty>();
        }

        /// <summary>
        /// Section name.
        /// </summary>
        public string Name { get; set; }

        /// <summary>
        /// Set the comment to display above this section.
        /// </summary>
        public string Comment { get; set; }

        /// <summary>
        /// Get a property value.
        /// </summary>
        /// <param name="name">Name of the property.</param>
        /// <returns>Value of the property or null if it doesn't exist.</returns>
        public string Get(string name)
        {
            if (_properties.ContainsKey(name))
                return _properties[name].Value;

            return null;
        }

        /// <summary>
        /// Set a property value.
        /// </summary>
        /// <param name="name">Name of the property.</param>
        /// <param name="value">Value of the property.</param>
        /// <param name="comment">A comment to display above the property.</param>
        public void Set(string name, string value, string comment = null)
        {
            if (string.IsNullOrWhiteSpace(value))
            {
                RemoveProperty(name);
                return;
            }

            if (!_properties.ContainsKey(name))
                _properties.Add(name, new IniProperty { Name = name, Value = value, Comment = comment });
            else
            {
                _properties[name].Value = value;
                if (comment != null)
                    _properties[name].Comment = comment;
            }
        }

        public void RemoveProperty(string propertyName)
        {
            if (_properties.ContainsKey(propertyName))
                _properties.Remove(propertyName);
        }

        /// <summary>
        /// Return the number of properties of this section.
        /// </summary>
        public int Count { get { return _properties.Count; } }

        public IEnumerator<IniProperty> GetEnumerator()
        {
            return _properties.Values.GetEnumerator();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }
    }

    public class IniFile
    {
        private readonly IList<IniSection> _sections;

        /// <summary>
        /// If True, writes extra spacing between the property name and the property value.
        /// (foo=bar) vs (foo = bar)
        /// </summary>
        public bool WriteSpacingBetweenNameAndValue { get; set; }

        /// <summary>
        /// Create a new IniFile instance.
        /// </summary>
        public IniFile()
        {
            _sections = new List<IniSection>();
        }

        /// <summary>
        /// Load an INI file from the file system.
        /// </summary>
        /// <param name="path">Path to the INI file.</param>
        public IniFile(string path) : this()
        {
            Load(path);
        }

        /// <summary>
        /// Load an INI file.
        /// </summary>
        /// <param name="reader">A TextReader instance.</param>
        public IniFile(TextReader reader) : this()
        {
            Load(reader);
        }

        private void Load(string path)
        {
            using (var file = new StreamReader(path))
                Load(file);
        }

        private void Load(TextReader reader)
        {
            IniSection section = null;

            string line;
            while ((line = reader.ReadLine()) != null)
            {
                line = line.Trim();

                // skip empty lines
                if (line == string.Empty)
                    continue;

                // skip comments
                if (line.StartsWith(";"))
                    continue;

                if (line.StartsWith("[") && line.EndsWith("]"))
                {
                    var sectionName = line.Substring(1, line.Length - 2);
                    section = _sections.SingleOrDefault(x => x.Name == sectionName);
                    if (section == null)
                    {
                        section = new IniSection(sectionName);
                        _sections.Add(section);
                    }
                    continue;
                }

                if (section != null)
                {
                    var keyValue = line.Split('=');
                    if (keyValue.Length != 2)
                        continue;

                    section.Set(keyValue[0].Trim(), keyValue[1].Trim());
                }
            }
        }

        /// <summary>
        /// Get a section by name. If the section doesn't exist, it is created.
        /// </summary>
        /// <param name="sectionName">The name of the section.</param>
        /// <returns>A section. If the section doesn't exist, it is created.</returns>
        public IniSection Section(string sectionName)
        {
            var section = _sections.SingleOrDefault(x => x.Name == sectionName);
            if (section == null)
            {
                section = new IniSection(sectionName);
                _sections.Add(section);
            }

            return section;
        }

        /// <summary>
        /// Get all sections.
        /// </summary>
        /// <returns>An array of all the sections.</returns>
        public IniSection[] Sections()
        {
            return _sections.ToArray();
        }

        /// <summary>
        /// Remove a section.
        /// </summary>
        /// <param name="sectionName">Name of the section to remove.</param>
        public void RemoveSection(string sectionName)
        {
            var section = _sections.SingleOrDefault(x => x.Name == sectionName);
            if (section != null)
                _sections.Remove(section);
        }

        /// <summary>
        /// Create a new INI file.
        /// </summary>
        /// <param name="path">Path to the INI file to create.</param>
        public void Save(string path)
        {
            using (var file = new StreamWriter(path))
                Save(file);
        }

        /// <summary>
        /// Create a new INI file.
        /// </summary>
        /// <param name="writer">A TextWriter instance.</param>
        public void Save(TextWriter writer)
        {
            foreach (var section in _sections)
            {
                if (section.Count == 0)
                    continue;

                if (section.Comment != null)
                    writer.WriteLine(";" + section.Comment);

                writer.WriteLine("[{0}]", section.Name);

                foreach (var property in section)
                {
                    if (property.Comment != null)
                        writer.WriteLine(";" + property.Comment);

                    var format = WriteSpacingBetweenNameAndValue ? "{0} = {1}" : "{0}={1}";
                    writer.WriteLine(format, property.Name, property.Value);
                }

                writer.WriteLine();
            }
        }
    }
}
