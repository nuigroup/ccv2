#region License, Terms and Conditions
//
// Jayrock - JSON and JSON-RPC for Microsoft .NET Framework and Mono
// Written by Atif Aziz (www.raboof.com)
// Copyright (c) 2005 Atif Aziz. All rights reserved.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 3 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
//
#endregion

namespace Jayrock.Json.Conversion.Converters
{
    #region Imports

    using System;
    using System.Diagnostics;
    using System.Globalization;
    using System.Text.RegularExpressions;
    using System.Xml;

    #endregion

    public class DateTimeImporter : ImporterBase
    {
        public DateTimeImporter() : 
            base(typeof(DateTime)) {}
        
        protected override object ImportFromString(ImportContext context, JsonReader reader)
        {
            Debug.Assert(context != null);
            Debug.Assert(reader != null);

            try
            {
                //
                // Date object, represented in JSON as "\/Date(number of ticks)\/". 
                // The number of ticks is a positive or negative long value 
                // that indicates the number of ticks (milliseconds) that 
                // have elapsed since midnight 01 January, 1970 UTC.
                // See also: http://msdn.microsoft.com/en-us/library/system.web.script.serialization.javascriptserializer.aspx
                //

                Match match = Regex.Match(reader.Text, @"\A \/ Date \( ([0-9]+) \) \/ \z",
                                  RegexOptions.IgnorePatternWhitespace
                                  | RegexOptions.IgnoreCase
                                  | RegexOptions.CultureInvariant);

                if (match.Success)
                {
                    DateTime time;

                    try
                    {
                        GroupCollection groups = match.Groups;

                        long ms = long.Parse(groups[1].Value, NumberStyles.None, CultureInfo.InvariantCulture);
                        time = UnixTime.ToDateTime(ms / 1000.0);
                    }
                    catch (OverflowException e)
                    {
                        throw StringError(e);
                    }

                    return ReadReturning(reader, time);
                }

                return ReadReturning(reader, XmlConvert.ToDateTime(reader.Text
                            #if !NET_1_0 && !NET_1_1
                            , XmlDateTimeSerializationMode.Local
                            #endif
                            ));
            }
            catch (FormatException e)
            {
                throw StringError(e);
            }
        }

        private static JsonException StringError(Exception e)
        {
            return new JsonException("Error importing JSON String as System.DateTime.", e);
        }

        protected override object ImportFromNumber(ImportContext context, JsonReader reader)
        {
            Debug.Assert(context != null);
            Debug.Assert(reader != null);

            string text = reader.Text;

            double time;

            try
            {
                time = Convert.ToDouble(text, CultureInfo.InvariantCulture);
            }
            catch (FormatException e)
            {
                throw NumberError(e, text);
            }
            catch (OverflowException e)
            {
                throw NumberError(e, text);
            }

            try
            {
                return ReadReturning(reader, UnixTime.ToDateTime(time));
            }
            catch (ArgumentException e)
            {
                throw NumberError(e, text);
            }
        }

        private static JsonException NumberError(Exception e, string text)
        {
            return new JsonException(string.Format("Error importing JSON Number {0} as System.DateTime.", text), e);
        }

        protected override JsonException GetImportException(string jsonValueType)
        {
            return new JsonException(string.Format("Found {0} where expecting a JSON String in ISO 8601 time format or a JSON Number expressed in Unix time.", jsonValueType));
        }
    }
}