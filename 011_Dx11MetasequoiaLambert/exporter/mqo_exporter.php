<?php
//
//// MetasequoiaLE R2.4 モデルデータの簡易エクスポータ
//

// 出力データフォーマット: (頂点数 32bit符号無整数)
//                         (x0 float)(y0 float)(z0 float)(法線nx0 float)(法線ny0 float)(法線nz0 float)
//                         (x1)(y1)(z1)(nx1)(ny1)(nz1)
//                         ...
//                         (面数 32bit符号無整数)
//                         (面0頂点0 32bit符号無整数)(面0頂点1 32bit符号無整数)(面0頂点2 32bit符号無整数)
//                         ...
//  これらのデータが "リトルエンディアン" で一続きになっています。

// ※注意!! このスクリプトで出力されるモデルデータは"超"暫定的で、
//          別のサンプルにある mqo_exporter.php とは違うデータを吐く可能性があります。
//          サンプルアプリケーション内でデータの妥当性チェックを行っているわけではありませんので、
//          使用には注意してください。


if ( !isset( $argc ) || $argc < 3 )
{
	echo "Usage: php(-cli) mqo_exporter.php [infile] [outfile]\n";
	exit;
}

// .mqo 読み込み
$content = @file_get_contents( $argv[1] );
if ( $content === false )
{
	echo "Error: Not found \"{$argv[1]}\"\n";
	exit;
}
echo "Input File: {$argv[1]} (" . strlen( $content ) . " bytes)\n";

// ShiftJIS→UTF-8
$content = mb_convert_encoding( $content, 'UTF-8', 'SJIS' );

// オブジェクト名を取得
$objects = GetMetasequoiaObjectNames( $content );
echo count( $objects ) . " objects:\n";
foreach ( $objects as $item )
{
	echo '   ' . mb_convert_encoding( $item, 'SJIS', 'UTF-8' ) . "\n";
}

// 頂点リスト・面リストを取得
$vertices_all = array();
$faces_all = array();
foreach ( $objects as $key => $val )
{
	echo 'Object #' . $key . ': ' . mb_convert_encoding( $val, 'SJIS', 'UTF-8' ) . "\n";

	$vertices = GetMetasequoiaObjectVertexList( GetMetasequoiaObject( $content, $val ) );
	echo '  Vertices: ' . count( $vertices ) . "\n";
	$faces = GetMetasequoiaObjectFaceList( GetMetasequoiaObject( $content, $val ) );
	echo '  Faces: ' . count( $faces ) . "\n";
	CalculateFaceNormalVector( $vertices, $faces );
	echo "  CalculateFaceNormalVector()\n";

	// 正規化された頂点法線ベクトルを求める
	echo "  Calculating vertex normal vector...\n";
	foreach ( $vertices as $key => $val )
	{
		$length = sqrt( $vertices[$key]['nx'] * $vertices[$key]['nx'] + 
		                $vertices[$key]['ny'] * $vertices[$key]['ny'] +
		                $vertices[$key]['nz'] * $vertices[$key]['nz'] );
		$vertices[$key]['nx'] /= $length;
		$vertices[$key]['ny'] /= $length;
		$vertices[$key]['nz'] /= $length;
	}

	// 面の頂点インデックスをずらす
	foreach ( $faces as $key => $val )
	{
		$faces[$key]['index'][0] += count( $vertices_all );
		$faces[$key]['index'][1] += count( $vertices_all );
		$faces[$key]['index'][2] += count( $vertices_all );
	}

	$vertices_all = array_merge( $vertices_all, $vertices );
	$faces_all = array_merge( $faces_all, $faces );
}

// 頂点リスト
echo "Packing vertex list...\n";
ob_start();
echo pack( 'V', count( $vertices_all ) );
foreach ( $vertices_all as $item )
{
	echo pack( 'f*', $item[ 'x'], $item[ 'y'], $item[ 'z'] );
	echo pack( 'f*', $item['nx'], $item['ny'], $item['nz'] );
}
$vertices_binary = ob_get_contents();
ob_end_clean();

// 面リスト
echo "Packing face list...\n";
ob_start();
echo pack( 'V', count( $faces_all ) );
foreach ( $faces_all as $item )
{
	echo pack( 'V*', $item['index'][0], $item['index'][1], $item['index'][2] );
}
$faces_binary = ob_get_contents();
ob_end_clean();

// ファイルに書き出し
echo "Writing to the file...\n";
$fp = fopen( $argv[2], 'wb' );
fwrite( $fp, $vertices_binary);
fwrite( $fp, $faces_binary);
fclose( $fp );



function GetMetasequoiaObjectNames( $content )
{
	$ret = array();
	if ( preg_match_all( '/Object "(.*?)"/', $content, $matched, PREG_SET_ORDER ) > 0 )
	{
		foreach ( $matched as $item )
		{
			$ret[] = $item[1];
		}
	}
	return $ret;
}


function GetMetasequoiaObject( $content, $object )
{
	$object_header = preg_quote( 'Object "' . $object . '" {', '/' );
	if ( preg_match( "/{$object_header}/", $content, $matched, PREG_OFFSET_CAPTURE ) )
	{
		$start    = $matched[0][1];   // $content 内で $object のオブジェクトが開始する位置
		$offset   = $start + strlen( $matched[0][0] );   // 中かっこを見つける際のオフセット
		$brackets = 1;   // 中かっこの入れ子具合

		while ( $brackets > 0 )
		{
			$left  = strpos( $content, '{', $offset );
			$right = strpos( $content, '}', $offset );

			// 右中かっこが足りなくなることはない
			if ( $right === false )
			{
				echo 'Error: In searching object "' . mb_convert_encoding( $object, 'SJIS', 'UTF-8' ) . "\"\n";
				exit;
			}

			// 左中かっこのほうが早く見つかった場合
			if ( $left !== false && $left < $right )
			{
				++$brackets;   // 入れ子具合増加
				$offset = $left + 1;
				continue;
			}

			// 左中かっこが見つからないか
			// 右中かっこのほうが早く見つかった場合はここに来る

			--$brackets;
			$offset = $right + 1;
		}

		return substr( $content, $start, $offset - $start );
	}

	// 見つかりませんでした
	echo 'Notice: Not found the object "' . mb_convert_encoding( $object, 'SJIS', 'UTF-8' ) . "\"\n";
	return '';
}


function GetMetasequoiaObjectVertexList( $object_content )
{
	// ※本来なら {(.*?)} を使いたいところだが、PHP 5.3.8 ではこうするとマッチしなくなってしまう。
	//   (PHP 4.4.9 ではマッチする)
	//   仕方ないので {(.*)} にしておいて、頂点数に達した際に break するようにした。

	if ( preg_match( '/vertex ([1-9][0-9]*) {(.*)}/s', $object_content, $matched ) )
	{
		$vertex_count = intval( $matched[1] );
		$vertex_list_str = preg_split( '/\\s*(?:\\x0D\\x0A|\\x0D|\\x0A)\\s*/', trim( $matched[2] ) );

		$vertex_list = array();
		foreach ( $vertex_list_str as $item )
		{
			// xyz 抽出
			if ( preg_match( '/^([-.0-9]+) ([-.0-9]+) ([-.0-9]+)$/', $item, $matched ) )
			{
				$vertex_list[] = array(
					'x' => floatval( $matched[1] ),
					'y' => floatval( $matched[2] ),
					'z' => floatval( $matched[3] )
				);
				// 頂点数のぶんデータを取ったらそこで終了
				if ( count( $vertex_list ) >= $vertex_count ) break;
			}
		}

		// 頂点数不一致を検出
		if ( count( $vertex_list ) !== $vertex_count )
		{
			echo "Error: In making a vertex list.\n";
			exit;
		}

		return $vertex_list;
	}

	// 頂点なし
	echo "Notice: There are no vertices.\n";
	return array();
}


function GetMetasequoiaObjectFaceList( $object_content )
{
	// ※本来なら {(.*?)} を使いたいところだが、PHP 5.3.8 ではこうするとマッチしなくなってしまう。
	//   (PHP 4.4.9 ではマッチする)
	//   仕方ないので {(.*)} にしておいて、頂点数に達した際に break するようにした。

	if ( preg_match( '/face ([1-9][0-9]*) {(.*)}/s', $object_content, $matched ) )
	{
		$face_count    = intval( $matched[1] );
		$face_list_str = preg_split( '/\\s*(?:\\x0D\\x0A|\\x0D|\\x0A)\\s*/', trim( $matched[2] ) );

		$face_list_indices = array();   // 面ごとの頂点インデックス配列
		foreach ( $face_list_str as $item )
		{
			// 頂点インデックス抽出
			if ( preg_match( '/V\\(([0-9]+) ([0-9]+) ([0-9]+)\\)/', $item, $matched ) )
			{
				$face_list_indices[] = array( intval($matched[1]), intval($matched[2]), intval($matched[3]) );
				// 面数ぶんのデータを取ったらそこで終了
				if ( count( $face_list_indices ) >= $face_count ) break;
			}
		}

		// 面数不一致を検出
		if ( count( $face_list_indices ) !== $face_count )
		{
			echo "Error: In making a face list.\n";
			exit;
		}

		// 面リスト生成
		$face_list = array();
		for ( $i = 0; $i < $face_count; ++$i )
		{
			$face_list[] = array(
				'index' => $face_list_indices[ $i ]
			);
		}
		return $face_list;
	}

	// 面なし
	echo "Notice: There are no faces.\n";
	return array();
}


function CalculateFaceNormalVector( &$vertices, $faces )
{
	foreach ( $faces as $key1 => $val1 )
	{
		// クロス積
		$a = array(
			'x' => $vertices[ $val1['index'][1] ]['x'] - $vertices[ $val1['index'][0] ]['x'],
			'y' => $vertices[ $val1['index'][1] ]['y'] - $vertices[ $val1['index'][0] ]['y'],
			'z' => $vertices[ $val1['index'][1] ]['z'] - $vertices[ $val1['index'][0] ]['z']
		);
		$b = array(
			'x' => $vertices[ $val1['index'][2] ]['x'] - $vertices[ $val1['index'][0] ]['x'],
			'y' => $vertices[ $val1['index'][2] ]['y'] - $vertices[ $val1['index'][0] ]['y'],
			'z' => $vertices[ $val1['index'][2] ]['z'] - $vertices[ $val1['index'][0] ]['z']
		);
		$a_cross_b = array(
			'x' => $a['y'] * $b['z'] - $a['z'] * $b['y'],
			'y' => $a['z'] * $b['x'] - $a['x'] * $b['z'],
			'z' => $a['x'] * $b['y'] - $a['y'] * $b['x']
		);
		// 正規化
		$length = sqrt( $a_cross_b['x']*$a_cross_b['x'] + $a_cross_b['y']*$a_cross_b['y'] + $a_cross_b['z']*$a_cross_b['z'] );
		$a_cross_b['x'] /= $length;
		$a_cross_b['y'] /= $length;
		$a_cross_b['z'] /= $length;

		// この面に属する頂点の法線ベクトルの総和を計算しておく
		foreach ( $val1['index'] as $key2 => $val2 )
		{
			if ( ! array_key_exists( 'nx', $vertices[ $val2 ] ) )
				$vertices[ $val2 ]['nx'] = 0.0;
			if ( ! array_key_exists( 'ny', $vertices[ $val2 ] ) )
				$vertices[ $val2 ]['ny'] = 0.0;
			if ( ! array_key_exists( 'nz', $vertices[ $val2 ] ) )
				$vertices[ $val2 ]['nz'] = 0.0;

			$vertices[ $val2 ]['nx'] += $a_cross_b['x'];
			$vertices[ $val2 ]['ny'] += $a_cross_b['y'];
			$vertices[ $val2 ]['nz'] += $a_cross_b['z'];
		}
	}
}
