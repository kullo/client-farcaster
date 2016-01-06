/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
.pragma library

// djb2 hashing algorithm (non-cryptographic)
function djb2(data) {
    var hash = 5381;
    for (var i = 0; i < data.length; ++i) {
		hash = ((hash << 5) + hash) + data.charCodeAt(i);
	}
	return hash;
}
