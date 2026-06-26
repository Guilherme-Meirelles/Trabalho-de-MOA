# Campanha de experimentos do SCP (AG + Busca Local).
# Gera Resultados/experimentos.csv com varias execucoes para o relatorio:
#   E1 - boxplots de GAP: N sementes por instancia, config calibrada, busca local A+B.
#   E2 - estudo de vizinhancas: modos A(1), B(2), A+B(3) nas instancias rapidas.
#   E3 - estudo de tamanho de populacao: pop in {20,50,100}.
# Uso:  powershell -File Resultados\run_experimentos.ps1
# Requer scp_run.exe ja compilado na raiz do repositorio.

$repo = Split-Path $PSScriptRoot -Parent
$exe  = Join-Path $repo "scp_run.exe"
$csv  = Join-Path $PSScriptRoot "experimentos.csv"

if (-not (Test-Path $exe)) { Write-Error "scp_run.exe nao encontrado em $repo. Compile primeiro."; exit 1 }

# Config calibrada (func/alpha vencedores da varredura) e limite de tempo por instancia.
$insts = @(
  @{ name="Teste_01"; best=557.44;  func=3; alpha=0.10; tl=5000  },
  @{ name="Teste_02"; best=537.89;  func=3; alpha=0.10; tl=5000  },
  @{ name="Teste_03"; best=517.58;  func=3; alpha=0.10; tl=5000  },
  @{ name="Teste_04"; best=1162.8;  func=3; alpha=0.10; tl=5000  },
  @{ name="Teste_05"; best=1020.12; func=3; alpha=0.10; tl=8000  },
  @{ name="Wren_01";  best=7856;    func=2; alpha=0.30; tl=10000 },
  @{ name="Wren_02";  best=13908;   func=1; alpha=0.20; tl=30000 },
  @{ name="Wren_03";  best=13780;   func=1; alpha=0.10; tl=30000 },
  @{ name="Wren_04";  best=58161;   func=3; alpha=0.10; tl=60000 }
)

"experimento;instancia;melhor_conhecido;custo;gap_pct;n_col;iteracoes;tempo_ms;limite_ms;viavel;func;alpha;semente;modo_bl;pop" | Out-File -FilePath $csv -Encoding utf8

function Run-One($exp, $inst, $seed, $func, $alpha, $pop, $modo, $tl) {
  $path = Join-Path $repo ("Tabela\" + $inst.name + ".dat")
  $out = & $exe $path $tl $seed $func $alpha $pop $modo
  $line = ($out | Select-String "^RESULTADO;" | Select-Object -First 1).Line
  if (-not $line) { Write-Host "  [FALHA] $($inst.name) seed=$seed"; return }
  $f = $line.Split(';')
  $custo = [double]$f[2]
  $gap = [math]::Round((($custo - $inst.best) / $inst.best) * 100, 4)
  $row = "$exp;$($inst.name);$($inst.best);$custo;$gap;$($f[3]);$($f[4]);$($f[5]);$($f[6]);$($f[7]);$($f[8]);$($f[9]);$($f[10]);$($f[11]);$($f[12])"
  $row | Out-File -FilePath $csv -Append -Encoding utf8
  Write-Host "  [$exp] $($inst.name) seed=$seed modo=$modo pop=$pop -> custo=$custo gap=$gap%"
}

# ----- E1: boxplots de GAP (N sementes, config calibrada, modo A+B) -----
Write-Host "=== E1: campanha de boxplots ==="
$N1 = 10
foreach ($inst in $insts) {
  for ($s = 1; $s -le $N1; $s++) { Run-One "E1" $inst $s $inst.func $inst.alpha 50 3 $inst.tl }
}

# ----- E2: estudo de vizinhancas (modos 1,2,3) nas instancias rapidas -----
Write-Host "=== E2: estudo de vizinhancas ==="
$N2 = 6
$rapidas = $insts | Where-Object { $_.name -like "Teste_*" -or $_.name -eq "Wren_01" }
foreach ($inst in $rapidas) {
  foreach ($modo in 1,2,3) {
    for ($s = 1; $s -le $N2; $s++) { Run-One "E2" $inst $s $inst.func $inst.alpha 50 $modo $inst.tl }
  }
}

# ----- E3: estudo de tamanho de populacao -----
Write-Host "=== E3: estudo de populacao ==="
$N3 = 6
$alvoPop = $insts | Where-Object { $_.name -eq "Teste_02" -or $_.name -eq "Wren_01" }
foreach ($inst in $alvoPop) {
  foreach ($pop in 20,50,100) {
    for ($s = 1; $s -le $N3; $s++) { Run-One "E3" $inst $s $inst.func $inst.alpha $pop 3 $inst.tl }
  }
}

Write-Host "FIM. CSV salvo em $csv"
