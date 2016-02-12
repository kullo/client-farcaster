/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
.pragma library

function mime2icon(mime) {
    if (mime === "application/pdf") return "application-pdf.svg";
    // Access (.mdb)
    if (mime === "application/x-msaccess") return "application-vnd.ms-access.svg";
    // Excel (.xls, .xlsx)
    if (mime === "application/vnd.ms-excel") return "application-vnd.ms-excel.svg";
    if (mime === "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet") return "application-vnd.ms-excel.svg";
    // Word (.doc, .docx)
    if (mime === "application/msword") return "application-msword.svg";
    if (mime === "application/vnd.openxmlformats-officedocument.wordprocessingml.document") return "application-msword.svg";
    // Powerpoint (.ppt, .pptx)
    if (mime === "application/vnd.ms-powerpoint") return "application-vnd.ms-powerpoint.svg";
    if (mime === "application/vnd.openxmlformats-officedocument.presentationml.presentation") return "application-vnd.ms-powerpoint.svg";
    // X
    if (mime === "application/x-cd-image") return "application-x-cd-image.svg";
    // audio
    if (mime === "audio/mpeg") return "audio-mpeg.svg";
    if (mime === "audio/mp4") return "audio-x-generic.svg";
    if (mime === "audio/wav") return "audio-x-wav.svg";
    if (mime === "audio/x-wav") return "audio-x-wav.svg";
    if (mime === "audio/x-ms-wma") return "audio-x-ms-wma.svg";
    if (mime === "audio/vorbis") return "audio-x-vorbis+ogg.svg";
    if (mime === "audio/ogg") return "audio-x-vorbis+ogg.svg";
    // archives
    if (mime === "application/7zip") return "application-7zip.svg";
    if (mime === "application/x-bzip") return "application-x-bzip.svg";
    if (mime === "application/x-bzip-compressed-tar") return "application-x-bzip-compressed-tar.svg";
    if (mime === "application/x-compressed-tar") return "application-x-compressed-tar.svg";
    if (mime === "application/x-gzip") return "application-x-gzip.svg";
    if (mime === "application/x-rar") return "application-x-rar.svg";
    if (mime === "application/zip") return "application-zip.svg";
    // images
    if (mime === "image/bmp") return "image-bmp.svg";
    if (mime === "image/gif") return "image-gif.svg";
    if (mime === "image/jpeg") return "image-jpeg.svg";
    if (mime === "image/png") return "image-png.svg";
    if (mime === "image/tiff") return "image-tiff.svg";
    // text
    if (mime === "text/plain") return "text-generic.svg";
    if (mime === "text/html") return "text-html.svg";
    if (mime === "text/css") return "text-css.svg";
    // OpenDocument documents
    // See http://de.wikipedia.org/wiki/OpenDocument#Dateiendungen_und_MIME-Typ
    if (mime === "application/vnd.oasis.opendocument.text") return "opendocument-text.png";
    if (mime === "application/vnd.oasis.opendocument.spreadsheet") return "opendocument-spreadsheet.png";
    if (mime === "application/vnd.oasis.opendocument.presentation") return "opendocument-presentation.png";
    if (mime === "application/vnd.oasis.opendocument.graphics") return "opendocument-graphics.png";
    if (mime === "application/vnd.oasis.opendocument.formula") return "opendocument-formula.png";

    // empty file
    if (mime === "application/x-zerosize") return "default.svg";

    // fallbacks
    if (mime.substring(0,6) === "audio/") return "audio-x-generic.svg";
    if (mime.substring(0,6) === "image/") return "image-x-generic.svg";
    if (mime.substring(0,5) === "text/") return "text-generic.svg";
    if (mime.substring(0,6) === "video/") return "video-x-generic.svg";

    console.warn("No file icon found for mime: '" + mime + "'");
    return "default.svg";
}
