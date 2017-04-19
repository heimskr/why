string			-> "\"" stringchar:* "\""								{% d => d[1].join("") %}

stringchar		-> [^\\"\n]												{% d => d[0] %}
				 | "\\" stringescape									{% d => JSON.parse(`"${d.join("")}"`) %}

stringescape	-> [A-F0-9] [A-F0-9]									{% d => `u00${d.join("")}` %}
				 | ["\\/bfnrt]											{% d => d[0] %}
				 | "u" [a-fA-F0-9] [a-fA-F0-9] [a-fA-F0-9] [a-fA-F0-9]	{% d => d.join("") %}
