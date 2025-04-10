<?php
$filename = "comments.txt";

$username = htmlspecialchars($_POST["username"] ?? "Anonymous");
$message = htmlspecialchars($_POST["message"] ?? "");
if ($username && $message) {
    if (strlen($username) > 12) $username = substr($username, 0, 12);
    if (strlen($message) > 24) $message = substr($message, 0, 36);
    $comment = "$username: $message\n";
    file_put_contents($filename, $comment, FILE_APPEND);
}

$successPage = file_get_contents("../guestbook.html");
echo $successPage;

