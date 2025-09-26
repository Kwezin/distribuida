# client_py.py
import grpc
import notas_pb2
import notas_pb2_grpc

def run(target="localhost:50052"):
    channel = grpc.insecure_channel(target)
    stub = notas_pb2_grpc.GerenciadorNotasStub(channel)

    print("Adicionando notas...")
    print(stub.AdicionarNota(notas_pb2.AdicionaNotaRequest(ra="789", cod_disciplina="FIS0001", ano=2025, semestre=1, nota=10.0)).msg)
    print(stub.AdicionarNota(notas_pb2.AdicionaNotaRequest(ra="789", cod_disciplina="MAT0001", ano=2025, semestre=1, nota=9.0)).msg)
    print(stub.AdicionarNota(notas_pb2.AdicionaNotaRequest(ra="101", cod_disciplina="FIS0001", ano=2025, semestre=1, nota=6.5)).msg)

    print("\nConsultando nota FIS0001 RA 789")
    resp = stub.ConsultarNota(notas_pb2.AlunoDisciplinaRequest(ra="789", cod_disciplina="FIS0001"))
    if resp.sucesso:
        n = resp.nota
        print("Encontrada:", n)
    else:
        print("Erro:", resp.msg_erro)

    print("\nAlterando MAT0001 RA 789 para 9.5")
    print(stub.AlterarNota(notas_pb2.AdicionaNotaRequest(ra="789", cod_disciplina="MAT0001", ano=2025, semestre=1, nota=9.5)).msg)

    print("\nCalculando média RA 789")
    m = stub.CalcularMedia(notas_pb2.AlunoRequest(ra="789"))
    if m.sucesso:
        print("Média:", m.media)
    else:
        print("Erro média:", m.msg_erro)

    print("\nListando notas RA 789 (stream)")
    for nota in stub.ListarNotasAluno(notas_pb2.AlunoRequest(ra="789")):
        print("Nota stream:", nota)

if __name__ == '__main__':
    run()
