using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Web;
using Jayrock.Json.Conversion;

using NuiApiWrapper;
using Jayrock.Json;
using Jayrock.Json.Conversion.Converters;
using System.Diagnostics;
using Jayrock;

namespace WrapperTest
{
    public abstract class CollectionImporterBase : ImporterBase
    {
        private readonly Type _elementType;

        public CollectionImporterBase(Type outputType, Type elementType) :
            base(outputType)
        {
            if (elementType == null) throw new ArgumentNullException("elementType");

            _elementType = elementType;
        }

        public Type ElementType
        {
            get { return _elementType; }
        }

        protected override object ImportFromArray(ImportContext context, JsonReader reader)
        {
            if (context == null) throw new ArgumentNullException("context");
            if (reader == null) throw new ArgumentNullException("reader");

            object collection = CreateCollection();

            reader.ReadToken(JsonTokenClass.Array);

            ImportElements(collection, context, reader);

            if (reader.TokenClass != JsonTokenClass.EndArray)
                throw new Exception("Implementation error.");

            reader.Read();
            return collection;
        }

        protected abstract object CreateCollection();
        protected abstract void ImportElements(object collection, ImportContext context, JsonReader reader);
    }

    public class CollectionImporter<Collection, Element> : CollectionImporterBase
    where Collection : ICollection<Element>, new()
    {
        public CollectionImporter() :
            base(typeof(Collection), typeof(Element)) { }

        protected override object CreateCollection()
        {
            return new Collection();
        }

        protected override void ImportElements(object collection, ImportContext context, JsonReader reader)
        {
            if (collection == null) throw new ArgumentNullException("collection");
            if (context == null) throw new ArgumentNullException("context");
            if (reader == null) throw new ArgumentNullException("reader");

            ImportElements((ICollection<Element>)collection, context, reader);
        }

        private static void ImportElements(ICollection<Element> collection, ImportContext context, JsonReader reader)
        {
            Debug.Assert(collection != null);
            Debug.Assert(context != null);
            Debug.Assert(reader != null);

            while (reader.TokenClass != JsonTokenClass.EndArray)
                collection.Add((Element)context.Import(typeof(Element), reader));
        }
    }

    public class ListImporter<T> : CollectionImporter<List<T>, T>
    where T : new() { }

    class Program
    {
        static void Main(string[] args)
        {
            ImportContext impctx = new ImportContext();

        }
    }
}
